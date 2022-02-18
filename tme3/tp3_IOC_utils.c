void
lcd_set_cursor(int x, int y) {
	int total_move = x * 20 + y; // on décale de (x lignes + y cases) cases

	int i;
	for(i = 0; i < total_move; i++) { // on décale le curseur d'une case à droiteele bon nombre de fois
		lcd_command(LCD_CURSORSHIFT | LCD_CS_DISPLAYMOVE | LCD_CS_MOVERIGHT);
	}

	// si le curseur ne s'affiche pas peut être décommenter
	//lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON);
}