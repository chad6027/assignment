#include "includes.h"

#define F_CPU	16000000UL	// CPU frequency = 16 Mhz
#define F_SCK 40000UL  // SCK 클록 값 = 40 Khz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define UCHAR unsigned char // UCHAR 정의
#define USHORT unsigned short // USHORT 정의
#define ON 1
#define OFF 0
#define ATS75_ADDR 0x98 // 0b10011000, 7비트를 1비트 left shift
#define ATS75_CONFIG_REG 1
#define ATS75_TEMP_REG 0
#define  TASK_STK_SIZE  OS_TASK_DEF_STK_SIZE
#define  N_TASKS        6

#define WARNING 700
#define DO 17
#define RE 43
#define MI 66
#define FA 77
#define SOL 96
#define LA 113
#define TI 129
#define UDO 136


OS_STK		TaskStk[N_TASKS][TASK_STK_SIZE];
char TaskData[N_TASKS];

OS_EVENT	*Mbox_adc;
OS_EVENT	*Queue_to_sensor;
OS_FLAG_GRP *Flag_done;
void* queue[3];
INT8U err;
volatile USHORT FndNum;
volatile USHORT state = OFF;
volatile USHORT warning_level;
const UCHAR note[8] = { DO, RE, MI, FA, SOL, LA, TI, UDO };

void InitAdc(void);
USHORT read_adc();
void InitBuzzer(void);
void InitFnd(void);
void InitLed(void);

void AdcTask(void* data);
void ValueDeliverTask(void* data);
void BuzzerTask(void* data);
void FndTask(void* data);
void FndDisplayTask(void* data);
void LedTask(void* data);



int main (void)
{
  OSInit();


  OS_ENTER_CRITICAL();
  TCCR0 = 0x07;
  TIMSK = _BV(TOIE0);
  TCNT0 = 256 - (CPU_CLOCK_HZ / OS_TICKS_PER_SEC / 1024);
  OS_EXIT_CRITICAL();
  
  Mbox_adc = OSMboxCreate((void*)0);
  Queue_to_sensor = OSQCreate(&queue[0], 3);
  Flag_done = OSFlagCreate(0x00, &err);

  INT8U i = 0;

  for (i = 0; i < N_TASKS; i++)
	  TaskData[i] = '0' + i;

  OSTaskCreate(AdcTask, (void *)&TaskData[0], (void *)&TaskStk[0][TASK_STK_SIZE - 1], 0);
  OSTaskCreate(ValueDeliverTask, (void *)&TaskData[1], (void *)&TaskStk[1][TASK_STK_SIZE - 1], 1);
  OSTaskCreate(BuzzerTask, (void *)&TaskData[2], (void *)&TaskStk[2][TASK_STK_SIZE - 1], 2);
  OSTaskCreate(FndTask, (void*)&TaskData[3], (void*)&TaskStk[3][TASK_STK_SIZE - 1], 3);
  OSTaskCreate(LedTask, (void*)&TaskData[4], (void*)&TaskStk[4][TASK_STK_SIZE - 1], 4);
  OSTaskCreate(FndDisplayTask, (void*)&TaskData[5], (void*)&TaskStk[5][TASK_STK_SIZE - 1], 5);

  OSStart();

  return 0;
}

ISR(TIMER2_OVF_vect)
{
	if (state == ON)
	{
		PORTB = 0x00;
		state = OFF;
	}
	else
	{
		PORTB = 0x10;
		state = ON;
	}
	TCNT2 = note[warning_level];
}


void InitAdc(void) {
	ADMUX = 0x00;
	// 00000000
	// REFS(1:0) = "00" AREF(+5V) 기준전압 사용
	// ADLAR = '0' 디폴트 오른쪽 정렬
	// MUX(4:0) = "00000" ADC0 사용, 단극 입력
	ADCSRA = 0x87;
	// 10000111
	// ADEN = '1' ADC를 Enable
	// ADFR = '0' single conversion 모드
	// ADPS(2:0) = "111" 프리스케일러 128분주
}

USHORT read_adc() {
	UCHAR adc_low, adc_high;
	USHORT value;
	ADCSRA |= 0x40; // ADC start conversion, ADSC = '1'
	while ((ADCSRA & (0x10)) != 0x10); // ADC 변환 완료 검사
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;

	return value;
}
void InitLed(void) {
	DDRA = 0xff;
}
void InitFnd(void) {
	//FND
	DDRC = 0xff;
	DDRG = 0x0f;
}
void InitBuzzer(void) {
	//Buzzer
	DDRB = 0x10;
	TCCR2 = 0x03;
	sei();
	TCNT2 = DO;
}

void AdcTask(void* data) {
	INT8U err;
	data = data;
	USHORT adc_value;
	InitAdc();

	while (1) {
		
		OS_ENTER_CRITICAL(); // critical section 진입
		adc_value = read_adc();
		OS_EXIT_CRITICAL();

		OSMboxPost(Mbox_adc, (void*)&adc_value); // ValueDeliverTask에게 adc_value 전달

		OSFlagPend(Flag_done, 0x3C, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err); // FndDisplayTask가 신호를 전달해주기 전까지 Block
	}
}

void ValueDeliverTask(void* data) {
	data = data;
	USHORT adc_value, i;
	INT8U err;
	while (1){
		adc_value = *(USHORT*)OSMboxPend(Mbox_adc, 0, &err);
		
		// Message queue에 값 전달
		for (i = 0; i < 3; i++) {
			OSQPost(Queue_to_sensor, (void*)&adc_value);
		}
	}
}

void BuzzerTask(void* data) {
	data = data;
	USHORT task_number = (USHORT)(*(char*)data - '0');
	USHORT adc_value;

	INT8U err;
	InitBuzzer();
	while (1) {
		adc_value = *(USHORT *) OSQPend(Queue_to_sensor, 0, &err);
		
		if (adc_value > WARNING) {
			OS_ENTER_CRITICAL();
			warning_level = ((adc_value - WARNING) / 100) * 2;
			TIMSK |= _BV(TOIE2);
			OS_EXIT_CRITICAL();
		}
		else {
			OS_ENTER_CRITICAL();
			TIMSK = _BV(TOIE0);
			OS_EXIT_CRITICAL();
		}

		OSFlagPost(Flag_done, _BV(task_number), OS_FLAG_SET, &err);
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}

void FndTask(void* data) {
	USHORT task_number = (USHORT)(*(char*)data - '0');
	data = data;
	USHORT adc_value;
	INT8U err;
	InitFnd();

	while (1) {
		adc_value = *(USHORT*)OSQPend(Queue_to_sensor, 0, &err);

		OS_ENTER_CRITICAL(); // critical section 진입
		FndNum = (USHORT)adc_value;
		OS_EXIT_CRITICAL();

		OSFlagPost(Flag_done, _BV(task_number), OS_FLAG_SET, &err);
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}


void LedTask(void* data) {
	data = data;
	USHORT task_number = (USHORT)(*(char*)data - '0');
	USHORT adc_value;
	USHORT i = 0;
	INT8U err;
	short tmp;
	InitLed();

	while (1) {
		adc_value = *(USHORT*)OSQPend(Queue_to_sensor, 0, &err);
		
		i = 0;
		tmp = (adc_value / 100);

		for (tmp; tmp >= 0; tmp--) i |= (1 << tmp); // LED 단계 설정
			
		PORTA = i;

		OSFlagPost(Flag_done, _BV(task_number), OS_FLAG_SET, &err);
		OSTimeDlyHMSM(0, 0, 0, 5);
	}
}

void FndDisplayTask(void* data) {

	USHORT task_number = (USHORT)(*(char*)data - '0');
	USHORT adc_value;
	USHORT i = 0;
	INT8U err;
	int fnd[4];
	const UCHAR digit[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f };
	const UCHAR fnd_sel[4] = { 0x01, 0x02, 0x04, 0x08 };

	while (1) {
		OS_ENTER_CRITICAL(); // critical section 진입
		adc_value = FndNum;
		OS_EXIT_CRITICAL();

		fnd[3] = digit[(adc_value / 1000) % 10];
		fnd[2] = digit[(adc_value / 100) % 10];
		fnd[1] = digit[(adc_value / 10) % 10];
		fnd[0] = digit[adc_value % 10];

		for (i = 0; i < 4; i++)
		{
			PORTC = fnd[i];
			PORTG = fnd_sel[i];
			_delay_ms(2);
		}

		OSFlagPost(Flag_done, _BV(task_number), OS_FLAG_SET, &err);
	}
}