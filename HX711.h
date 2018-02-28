// HX711.h

#ifndef HX711_h
#define HX711_h


//enum class for amplifier modes (channel/gain combination)
enum class HX711_MODE
  {
    NONE = 0,         //default mode with no channel or gain
    A_128 = 1,        //channel A gain 128
    A_64 = 3,         //channel B gain 32
    B_32 = 2          //channel A gain 64
  };

class HX711
{
private:
  const uint8_t gains_[4] = {0, 128, 32, 64}; //hardcoded gains
  
  uint8_t pd_sck_;	// Power Down and Serial Clock Input Pin
  uint8_t dout_;        // Serial Data Output Pin
  HX711_MODE mode_ = HX711_MODE::NONE;   //default mode of NONE to force user to set it
  long offset_ = 0;	// used for tare weight
  float scale_ = 1;	// used to return weight in grams, kg, ounces, whatever

public:

  HX711() = default;

  // define clock and data pin, channel, and mode
  HX711(uint8_t dout, uint8_t pd_sck, HX711_MODE mode);

  virtual ~HX711() = default;

  // Allows to set the pins and gain later than in the constructor
  void begin(uint8_t dout, uint8_t pd_sck, HX711_MODE mode);

  // check if HX711 is ready
  // from the datasheet: When output data is not ready for retrieval, digital output pin dout_ is high. Serial clock
  // input pd_sck_ should be low. When dout_y goes to low, it indicates data is ready for retrieval.
  bool is_ready();

  //sets the mode to change channel/gain. See the HX711_MODe enum class for options
  void set_mode(HX711_MODE mode);

  // waits for the chip to be ready and returns a reading
  long read();

  // returns an average reading; times = how many times to read
  long read_average(uint8_t times = 10);

  // returns (read_average() - offset_), that is the current value without the tare weight; times = how many readings to do
  double get_value(uint8_t times = 1);

  // returns get_value() divided by scale_, that is the raw value divided by a value obtained via calibration
  // times = how many readings to do
  float get_units(uint8_t times = 1);

  // set the offset_ value for tare weight; times = how many times to read the tare value
  void tare(uint8_t times = 10);

  // set the scale_ value; this value is used to convert the raw data to "human readable" data (measure units)
  void set_scale(float scale = 1.f);

  // get the current scale_
  float get_scale();

  // get the channel's gain
  uint8_t get_gain();

  // set offset_, the value that's subtracted from the actual reading (tare weight)
  void set_offset(long offset = 0);

  // get the current offset_
  long get_offset();

  // puts the chip into power down mode
  void power_down();

  // wakes up the chip after power down mode
  void power_up();
};

#endif /* HX711_h */
