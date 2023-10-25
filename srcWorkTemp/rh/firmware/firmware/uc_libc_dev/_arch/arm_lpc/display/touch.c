static u16 samples[TOUCH_SAMPLES][2];
static void select(int ad);
static void tiramedia(void);

// fun��o chamada constantemente para atualizar a leitura do touch
// Isto pode ser feito na interrup��o de um timer ou loop
void touch_Tick(void) {
	static int estado = 0;
	static int count = -1;
	u8 adc_channel;

	if (estado) adc_channel = TOUCH_Y0_AD1; else adc_channel = TOUCH_X0_AD0;

	// hora de trocar?
	if (count >= TOUCH_SAMPLES) {
		//faz a media
		if (estado) tiramedia();
		//troca o estado
		estado ^= 1;
		//ignora a primeira medi��o
		count = -1;
		select(estado);
		return;
	}

	//guarda a amostra (armazena valor entre 0..1023, por isso div 4)
	if (count>=0) {
		// A QUANTIDADE DE TICKS N�O DEVE SER MAIS R�PIDO DO QUE O TEMPO DE CONVERS�O
		adc0_Read(adc_channel, &samples[count][estado]);
		//guarda a amostra (armazena valor entre 0..1023, por isso div 4)
		samples[count][estado] = samples[count][estado]/4;
	}

	count++; 	//avan�a o contador
	adc0_Start(adc_channel, AD0_START_NOW, 0);
}

static void tiramedia(void) {
	static int xx=0,yy=0;
	int i,x,y, qtd;
	x = 0;
	y = 0;
	qtd = 0;

	// fazer somat�ria
	for (i=0; i<TOUCH_SAMPLES; i++) {
		// somente ira participar dos caculos os momentos que o usu�rio pressionou o touch
		// isto para eliminar as leituras no momento que o usu�rio n�o teclou quando dado inicio a varredura
		if ( (samples[i][0] > TOUCH_LIMIT_AD) && (samples[i][1] > TOUCH_LIMIT_AD) ) {
			x += samples[i][0];
			y += samples[i][1];
			qtd++;
		}
	}

	if (qtd == 0) {
		touch.touchx = 0;
		touch.touchy = 0;
		return;
	}

	// tirar m�dia
	x = x/qtd;
	y = y/qtd;

	// somente ser� capturado o touch se a leitura anterior esteja dentro do ranger da leitura anteriror
	if ((abs(xx-x)<5)&&(abs(yy-y)<5)) {
		touch.touchx = x; // touchx = x;
		touch.touchy = y; // touchy = y;
	}
	xx = x;
	yy = y;
}

