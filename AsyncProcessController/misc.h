bool all_zero(byte* sarr)
{
        for(int i = 0; i < TABLE_SLOTS; i++)
                if(sarr[i] == true)
                        return false;

        return true;
}

#define __digitalPinToPortReg(P) \
(((P) >= 22 && (P) <= 29) ? &PORTA : \
((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : \
(((P) >= 30 && (P) <= 37) ? &PORTC : \
((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : \
((((P) <= 3) || (P) == 5) ? &PORTE : \
(((P) >= 54 && (P) <= 61) ? &PORTF : \
((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : \
((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : \
(((P) == 14 || (P) == 15) ? &PORTJ : \
(((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))

#define __digitalPinToBit(P) \
(((P) >=  7 && (P) <=  9) ? (P) - 3 : \
(((P) >= 10 && (P) <= 13) ? (P) - 6 : \
(((P) >= 22 && (P) <= 29) ? (P) - 22 : \
(((P) >= 30 && (P) <= 37) ? 37 - (P) : \
(((P) >= 39 && (P) <= 41) ? 41 - (P) : \
(((P) >= 42 && (P) <= 49) ? 49 - (P) : \
(((P) >= 50 && (P) <= 53) ? 53 - (P) : \
(((P) >= 54 && (P) <= 61) ? (P) - 54 : \
(((P) >= 62 && (P) <= 69) ? (P) - 62 : \
(((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : \
(((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : \
(((P) == 19) ? 2 : \
(((P) == 5 || (P) == 6 || (P) == 18) ? 3 : \
(((P) == 2) ? 4 : \
(((P) == 3 || (P) == 4) ? 5 : 7)))))))))))))))

inline void fast_toggle(int pin)
{
        uint8_t oldSREG = SREG;
        cli();
        *__digitalPinToPortReg(pin) ^= (1 << __digitalPinToBit(pin));
        SREG = oldSREG;
}