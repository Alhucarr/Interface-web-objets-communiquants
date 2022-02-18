void write4lines(const char *txt) {
    int i;
    int a[] = { 0, 0x40, 0x14, 0x54 };

    for(i = 0; (i < strlen(txt)) && (i < 20); i++) {
        lcd_command(LCD_SETDDRAMADDR + a[0]);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[1]);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[2]);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[3]);
        lcd_data(txt[i]);
    }
}