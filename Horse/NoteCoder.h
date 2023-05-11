class NoteCoder {
public:
  int step = 128;
  int startMessage = 4096 - step;
  int endMessage = startMessage - step;

  NoteCoder();

  void encodeDirection(int * frequencies, float direction);
  float decodeDirection(int * frequencies);
};