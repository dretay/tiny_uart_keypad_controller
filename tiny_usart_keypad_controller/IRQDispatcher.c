#include "IRQDispatcher.h"

static volatile Subscription subscriptions[IRQD_NUM_SUBSCRIPTIONS];
static u16 subscriptions_usage_mask = 0;
static volatile u8 pcint0_last_state;
static volatile u8 pcint1_last_state;
static volatile u8 pcint2_last_state;

static u8 get_unused_idx() {
	u8 i = 0;
	for (i = 0; i < IRQD_NUM_SUBSCRIPTIONS; i++) {
		if (bitRead(subscriptions_usage_mask, i) == 0) {
			bitSet(subscriptions_usage_mask, i);
			return i;
		}
	}
	return -1;
}
static bool idx_valid(u8 idx) {
	return idx >= 0 && idx < IRQD_NUM_SUBSCRIPTIONS;
}
static s8 find_gpio_pin(u8 GPIO_Pin) {
	
	u8 i = 0;
	for (i = 0; i < IRQD_NUM_SUBSCRIPTIONS; i++) {
		if (bitRead(subscriptions_usage_mask, i) == 1) {
			if (subscriptions[i].gpio_pin == GPIO_Pin) {
				return i;
			}
		}
	}
	return -1;
}

static bool subscribe(u8 GPIO_Pin, void* handler) {
	s8 idx = get_unused_idx();
	if (idx_valid(idx)) {
		subscriptions[idx].callback = handler;
		subscriptions[idx].gpio_pin = GPIO_Pin;
		return true;
	}
	return false;
}
static bool unsubscribe(u8 GPIO_Pin) {
	u8 idx = find_gpio_pin(GPIO_Pin);
	if (idx_valid(idx)) {
		subscriptions[idx].callback = NULL;
		subscriptions[idx].gpio_pin = 0;
		return true;
	}
	return false;
}

static void init(void){
	//initialize state of GPIO registers
	pcint0_last_state=PINB;
	pcint1_last_state=PINA;
	pcint2_last_state=PIND;
	
	//enable pin change interrupts in the general mask register
	GIMSK |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
	
	//enable pci for relevant port b (pcmsk), port a(pcmsk1), and port d(pcmsk2) pins
	PCMSK |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2)| (1 << PCINT3)| (1 << PCINT4);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9); //porta
	PCMSK2 |= (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15)| (1 << PCINT16)| (1 << PCINT17);
	
	//enable interrupts
	sei();
}

static void external_irq_callback(u8 GPIO_Pin){
	u8 idx = find_gpio_pin(GPIO_Pin);
	if (idx_valid(idx))
	{		
		subscriptions[idx].callback(GPIO_Pin);
	}
}

// PCI0 will trigger if any enabled PCINT7..0 pin toggles. (everything on port B)
ISR(PCINT0_vect){
	u8 temp = pcint0_last_state^PINB;
	pcint0_last_state = PINB;
	if ((temp & _BV(0))) external_irq_callback(0); //B0
	if ((temp & _BV(1))) external_irq_callback(1); //B1
	if ((temp & _BV(2))) external_irq_callback(2); //B2
	if ((temp & _BV(3))) external_irq_callback(3); //B3
	if ((temp & _BV(4))) external_irq_callback(4); //B4
}

//PCI1 will trigger if any enabled PCINT10..8 pin toggles. (everything on port A)
ISR(PCINT1_vect) {
	u8 temp = pcint1_last_state^PINA;
	pcint1_last_state = PINA;
	if ((temp & _BV(0))) external_irq_callback(10);//A0
	if ((temp & _BV(1))) external_irq_callback(11);//A1
}

//PCI2 will trigger, if any enabled PCINT17..11 pin toggles (everything on port D)
ISR(PCINT2_vect) {
	u8 temp = pcint2_last_state^PIND;
	pcint2_last_state = PIND;
	if ((temp & _BV(2))) external_irq_callback(5);//D2
	if ((temp & _BV(3))) external_irq_callback(6);//D3
	if ((temp & _BV(4))) external_irq_callback(7);//D4
	if ((temp & _BV(5))) external_irq_callback(8);//D5
	if ((temp & _BV(6))) external_irq_callback(9);//D6
}


const struct irqdispatcher IRQDispatcher = {
	.init = init,
	.subscribe = subscribe,
	.unsubscribe = unsubscribe
};