
#ifndef DYNAMIXEL_INTERFACE_IMPL_H
#define DYNAMIXEL_INTERFACE_IMPL_H

#include "DynamixelInterface2.h"
#include <Arduino.h>

#if defined (__AVR__)
	#include <SoftwareSerial.h>
#endif /* __AVR__ */

/* 
 * Set UART direction pins so that serial port messages 
 * and other devices on line do not interfere
*/
# if defined (__AVR_ATmega2560__)
# 	ifndef DXL_DIR_TXD_PIN
# 		define DXL_DIR_TXD_PIN 43
# 	endif
# 	ifndef DXL_DIR_RXD_PIN
# 		define DXL_DIR_RXD_PIN 42
# 	endif
# elif defined (__AVR_ATmega328P__)
# 	ifndef DXL_DIR_TXD_PIN
# 		define DXL_DIR_TXD_PIN 2
# 	endif
# 	ifndef DXL_DIR_RXD_PIN
# 		define DXL_DIR_RXD_PIN 2
# 	endif
# elif defined (__AVR_ATmega32U4__)
# 	ifndef DXL_DIR_TXD_PIN
# 		define DXL_DIR_TXD_PIN -1
# 	endif
# 	ifndef DXL_DIR_RXD_PIN
# 		define DXL_DIR_RXD_PIN -1
# 	endif
# elif defined (ESP32)
# 	ifndef DXL_DIR_TXD_PIN
# 		define DXL_DIR_TXD_PIN -1
# 	endif
# 	ifndef DXL_DIR_RXD_PIN
# 		define DXL_DIR_RXD_PIN -1
# 	endif
# elif defined (__OPENCM904__)
#	ifndef DXL_DIR_TXD_PIN
# 		define DXL_DIR_TXD_PIN 28
# 	endif
# 	ifndef DXL_DIR_RXD_PIN
# 		define DXL_DIR_RXD_PIN 28
# 	endif
# endif

/*
 * Set serial port for Dynamixel interface
*/

# if defined (__AVR_ATmega2560__) 	
#	ifndef DXL_SERIAL_PORT
# 		define DXL_SERIAL_PORT Serial1
# 	endif
# elif defined (__AVR_ATmega328P__)
#	ifndef DXL_SERIAL_PORT
# 		define DXL_SERIAL_PORT Serial
# 	endif
# elif defined (__AVR_ATmega32U4__)
#	ifndef DXL_SERIAL_PORT
# 		define DXL_SERIAL_PORT Serial1
# 	endif
# elif defined (ESP32)
#	ifndef DXL_SERIAL_PORT
# 		define DXL_SERIAL_PORT Serial
# 	endif
# elif defined (__OPENCM904__)
#	ifndef DXL_SERIAL_PORT
# 		define DXL_SERIAL_PORT Serial1
# 	endif
# endif

template <class T>
class DynamixelInterfaceImpl : public DynamixelInterface
{
private:
    /** \brief Switch stream to read (receive)) mode */
    void readMode();

    /** \brief Switch stream to write (send) mode */
    void writeMode();

    unsigned long baud;

public:
    /**
	 * \brief Constructor
	 * \param[in] aStream : Stream controller that abstract real stream
	 * \param[in] aTxPin : Pin number of the Tx pin
	*/
	DynamixelInterfaceImpl(T *aStream, uint8_t aTxPin);

    /**
     * \brief Destructor
     * Delete stream if it is owned by the instance
     */
    ~DynamixelInterfaceImpl();

    /**
	 * \brief Start interface
	 * \param[in] aBaud : Baudrate
	 * \param[in] aStreamCustom : Pointer to custom serial port instance for Dynamixel interface
	 * \param[in] aTxDirCustom : Custom Tx DIR pin for Dynamixel interface
	 * \param[in] aRxDirCustom : Custom Rx DIR pin for Dynamixel interface
	 * 
	 * Start the interface with desired baudrate, call once before using the interface
	*/
	void begin(unsigned long aBaud,
				void *aStream = &DXL_SERIAL_PORT,
				int8_t aTxDirPin = DXL_DIR_TXD_PIN,
				int8_t aRxDirPin = DXL_DIR_RXD_PIN,
                int8_t aTxPin = -1,
                int8_t aRxPin = -1);

    /**
     * \brief Change timeOut
     * \param[in] timeOut : ms
     */
    void setTimeOut(uint16_t timeOut);

    /**
     * \brief Hardware specific preparations before transaction
     *
     * Reinit interface for dynamixel (flush, set baudrate)
     * for case when interface shared with other functional
     */
    void prepareTransaction();

    /**
     * \brief Hardware specific cleaning after transaction
     *
     * Reinit interface to its original state (flush, set baudrate)
     * for case when interface shared with other functional
     */
    void endTransaction();

    /**
     * \brief Send a packet on bus
     * \param[in] aPacket : Packet to send
     *
     * The function wait for the packet to be completly sent (using Stream.flush)
     */
    void sendPacket(const DynamixelPacket &aPacket);

        /**
     * \brief Send a packet on bus
     * \param[in] aPacket : Packet to send
     *
     * The function wait for the packet to be completly sent (using Stream.flush)
     */
    void sendPacket2(DynamixelPacket2 &aPacket);

    /**
     * \brief Receive a packet on bus
     * \param[out] aPacket : Received packet. mData field must be previously allocated
     * \param[in] answerSize : the size of the memory allocated to the mData field
     *
     * The function wait for a new packet on the bus.
     * Timeout depends of timeout of the underlying stream.
     * Return error code in case of communication error (timeout, checksum error, ...)
     */
    void receivePacket(DynamixelPacket &aPacket, uint8_t answerSize = 0);

        /**
     * \brief Receive a packet on bus
     * \param[out] aPacket : Received packet. mData field must be previously allocated
     * \param[in] answerSize : the size of the memory allocated to the mData field
     *
     * The function wait for a new packet on the bus.
     * Timeout depends of timeout of the underlying stream.
     * Return error code in case of communication error (timeout, checksum error, ...)
     */
    void receivePacket2(DynamixelPacket2 &aPacket, uint16_t answerSize = 0 , uint8_t mode = RECEIVE_NORMAL);

    /**
     * \brief Stop interface
     */
    void end();

    static const uint8_t NO_DIR_PORT = 255;

private:
    /* 
	 * Changing serial stream reference to pointer for
	 * making this stream changable in begin() method
	*/
	// T &mStream;
	T *mStream;

	int8_t mTxDirPin;
	int8_t mRxDirPin;

protected:
    const uint8_t mTxPin;
};

class HardwareDynamixelInterface : public DynamixelInterfaceImpl<HardwareSerial>
{
public:
    HardwareDynamixelInterface(HardwareSerial *aSerial);
    ~HardwareDynamixelInterface();
};

#endif
