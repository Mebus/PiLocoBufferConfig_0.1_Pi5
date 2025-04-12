#ifndef _LCD_TOPICS_H_
#define _LCD_TOPICS_H_


#include <string.h> // memset(), strcat(), strcpy() strlen()
#include <stdio.h>  // sprintf()
#include "SyncObjects.h" // CSyncMutex, CMutexUser



class CTopic
{
public:
	static const int WIDTH = 16;
	// Mutual exclusion for access of all Topic instances between event sources and the
	// LCD thread(s) that reads it and feeds the display
	static CSyncMutex LcdMutex;
protected:
	char m_Output[WIDTH+1];
public:
	bool m_Request;
	CTopic()
	:	m_Request(false)
	{ }
	virtual ~CTopic()
	{ }
	virtual const char* Get()=0;
};

class CValue : public CTopic
{
protected:
	const char* m_Name;
	virtual void GetValue(char* target)=0;
public:
	CValue(const char* name)
		: m_Name(name)
	{ }
	~CValue()
	{ }
	virtual const char* Get()
	{
		char value[WIDTH+1];
		GetValue(value);
		size_t numLen = strlen(value);
		size_t nameLen = strlen(m_Name);
		char* start=&m_Output[0];
		size_t overlap = 0;
		if (numLen+nameLen <= WIDTH-2)
		{// enough space for name plus ": " plus value
			start = &m_Output[WIDTH-2-numLen-nameLen];
		}
		else
		{// only ":" or less fits between name and value
			overlap = nameLen+numLen+2 - WIDTH;
		}

		memset(m_Output, ' ', sizeof(m_Output));
		strcpy(start, m_Name);
		strcat(start, ": ");
		start = &m_Output[strlen(m_Output) - overlap];
		strcpy(start, value);
		if (strlen(m_Output) != WIDTH) 
			return "error";
		return m_Output;
	}
};

class CDefault : public CValue
{
protected:
	const char* m_Value;
public:
	CDefault(const char* name, const char* value)
		: CValue(name), m_Value(value)
	{}

	virtual void GetValue(char* target)
	{
		strcpy(target, m_Value);
	}
};

class CCounter : public CValue
{
protected:
	unsigned int m_Counter;

public:
	CCounter(const char *name)
	: CValue(name), m_Counter(0)
	{}

	void SetValue(unsigned int newValue)
	{
		CMutexUser mu(LcdMutex);
		m_Counter = newValue;
		m_Request = true;
	}

	void Increment()
	{
		CMutexUser mu(LcdMutex);
		m_Counter++;
		m_Request = true;
	}

	virtual void GetValue(char* target)
	{
		sprintf(target, "%u", m_Counter);
	}
};

class CConnect : public CCounter
{
public:
	CConnect()
		: CCounter("Connections")
	{}

	void Connected(unsigned int number)
	{
		m_Name = "Connected";
		SetValue(number);
	}

	void Disconnected(unsigned int number)
	{
		m_Name = "Disconnected";
		SetValue(number);
	}
};

class CTxRx : public CTopic
{
protected:
	unsigned int m_Rx;
	unsigned int m_Tx;

public:
	CTxRx()
		: m_Rx(0), m_Tx(0)
	{ }

	void IncrementTx()
	{
		CMutexUser mu(LcdMutex);
		m_Tx++;
		m_Request = true;
	}
	void IncrementRx()
	{
		CMutexUser mu(LcdMutex);
		m_Rx++;
		m_Request = true;
	}

	virtual const char* Get()
	{
		char rx[11], tx[11];
		sprintf(rx, "%u", m_Rx);
		sprintf(tx, "%u", m_Tx);
		int rlen = static_cast<int>( strlen(rx) ); // printf format with "*" expects int
		int tlen = static_cast<int>( strlen(tx) );
		int len = rlen+tlen;

		#ifdef __GNUC__ // don't know why we get this warnings in release build: ignore
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wformat-overflow"
		#endif
		if (len < WIDTH-8)
			sprintf(m_Output, "TX: %-*sRX: %s", WIDTH-rlen-8, tx, rx);
		else if (len < WIDTH-6)
			sprintf(m_Output, "TX:%-*sRX:%s", WIDTH-rlen-6, tx, rx);
		else if (len <WIDTH-4)
			sprintf(m_Output, "TX%-*sRX%s", WIDTH-rlen-4, tx, rx);
		else if (len <WIDTH-2)
			sprintf(m_Output, "T%-*sR%s", WIDTH-rlen-2, tx, rx);
		else
			sprintf(m_Output, "RX: %-*s", WIDTH-4, rx);
		#ifdef __GNUC__
			#pragma GCC diagnostic pop
		#endif

		return m_Output;
	}
};


#endif // _LCD_TOPICS_H_
