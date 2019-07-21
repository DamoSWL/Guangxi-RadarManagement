unsigned long Float2Deg(float fdeg)
{
	if( fdeg < 0  ) fdeg = 360.f + fdeg;
	unsigned long ideg = fdeg / 0.02197265625 ;// (fdeg / (360/2^14)) 
	unsigned char low,high ;
	low = (unsigned char)(ideg % 128) ;
	high = (unsigned char)(ideg / 128) ;
	ideg = ((high << 8) & 0xFF00) + (low & 0xFF);  
	return ideg ;
}

float Deg2Float(short data)
{
	char *pData = (char *)&data;
	return pData[0] *0.02197265625 + pData[1] * 2.8125;
}