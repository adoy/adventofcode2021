#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define BUFFER 1400

typedef struct packet {
  int version;
  int type;
  union {
    long literal;
    struct packet *subPackets;
  };
  struct packet *next;
} packet_t;

char *hexStringToBinary(char *str) {
  size_t length = strlen(str);
  size_t i;
  char *result = (char *)malloc(sizeof(char) * length * 4);
  for (i = 0; i < length; i++) {
    int8_t v;
    if (str[i] >= '0' && str[i] <= '9') {
      v = str[i] - '0';
    } else if (str[i] >= 'A' && str[i] <= 'F') {
      v = str[i] - 55;
    } else {
      break;
    }
    result[i * 4] = (0b1000 & v) ? '1' : '0';
    result[i * 4 + 1] = (0b0100 & v) ? '1' : '0';
    result[i * 4 + 2] = (0b0010 & v) ? '1' : '0';
    result[i * 4 + 3] = (0b0001 & v) ? '1' : '0';
  }
  result[i * 4] = '\0';

  return result;
}

static int readBinary(char **str, int8_t length) {
  int result = 0;
  while (length) {
    result <<= 1;
    if (**str == '1') {
      result |= 1;
    }
    length--;
    (*str)++;
  }
  return result;
}

void freePacket(packet_t *packet) {
  if (packet->type != 4) {
    packet_t *sub = packet->subPackets;
    packet_t *next;
    while (sub) {
      next = sub->next;
      freePacket(sub);
      sub = next;
    }
  }
  free(packet);
}

packet_t *parse(char **str) {
  packet_t *packet = (packet_t *)malloc(sizeof(packet_t));
  packet->version = readBinary(str, 3);
  packet->type = readBinary(str, 3);
  packet->next = NULL;

  switch (packet->type) {
    case 4:
      packet->literal = 0L;
      do {
        long v = (long) readBinary(str, 5);
        packet->literal |= (0b01111) & v;
        if (0b10000 & v) {
          packet->literal <<= 4;
        } else {
          break;
        }
      } while (1);
      break;
    default:
      packet->subPackets = NULL;
      if (0 == readBinary(str, 1)) {
        int l = readBinary(str, 15);
        char *substr = *str;
        (*str) += l;
        while (substr < *str) {
          packet_t *subpacket = parse(&substr);
          subpacket->next = packet->subPackets;
          packet->subPackets = subpacket;
        }
      } else {
        int l = readBinary(str, 11);
        for (; l > 0; l--) {
          packet_t *subpacket = parse(str);
          subpacket->next = packet->subPackets;
          packet->subPackets = subpacket;
        }
      }
      break;
  }

  return packet;
}

int sumVersions(packet_t *packet) {
  int sum = packet->version;
  if (packet->type != 4) {
    packet_t *sub = packet->subPackets;
    while (sub) {
      sum += sumVersions(sub);
      sub = sub->next;
    }
  }
  return sum;
}

long computeValue(packet_t *packet) {
  long r = 0L;
  packet_t *sub;
  switch (packet->type) {
    case 0:
      sub = packet->subPackets;
      while (sub) {
        r += computeValue(sub);
        sub = sub->next;
      }
      break;
    case 1:
      r = 1L;
      sub = packet->subPackets;
      while (sub) {
        r *= computeValue(sub);
        sub = sub->next;
      }
      break;
    case 2:
      sub = packet->subPackets;
      r = computeValue(sub);
      while ((sub = sub->next)) {
        long v = computeValue(sub);
        if (v < r)
          r = v;
      }
      break;
    case 3:
      sub = packet->subPackets;
      r = computeValue(sub);
      while ((sub = sub->next)) {
        long v = computeValue(sub);
        if (v > r)
          r = v;
      }
      break;
    case 4:
      r = packet->literal;
      break;
    case 5:
      r = computeValue(packet->subPackets) <
          computeValue(packet->subPackets->next);
      break;
    case 6:
      r = computeValue(packet->subPackets) >
          computeValue(packet->subPackets->next);
      break;
    case 7:
      r = computeValue(packet->subPackets) ==
          computeValue(packet->subPackets->next);
      break;
  }
  return r;
}

int main() {
  char line[BUFFER];
  char *input = NULL;
  int result1 = 0;
  long result2 = 0L;

#ifdef BENCH
  clock_t start = clock();
#endif

  if (fgets(line, BUFFER, stdin)) {
    input = hexStringToBinary(line);
  }
  if (input) {
    char *str = input;
    packet_t *packet = parse(&str);
    result1 = sumVersions(packet);
    result2 = computeValue(packet);
    free(input);
    freePacket(packet);
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif
  printf("Result1: %d\nResult2: %ld\n", result1, result2);

  return 0;
}
