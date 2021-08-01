

#include <Adafruit_BME280.h>
#include <Wire.h>

class EnvSensor {
private:
  TwoWire *wire_;
  Adafruit_BME280 bme;

public:
  explicit EnvSensor(TwoWire *wire) : wire_(wire) {}
  void Start(void);
};