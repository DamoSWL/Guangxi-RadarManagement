#ifndef _DATTwo_
#define _DATTwo_

//�״�ԭʼ���ݸ�ʽ��VOLɨ�����ݸ�ʽ��PPI��RHIɨ�����ݸ�ʽ��
#pragma pack(push, 1)
//�ļ���ʶ
typedef struct _RADAR_FILE_IDENTIFICATION  //��12��
{
	char FileID[4]; //�״����ݱ�ʶ��ԭʼ���ݱ�ʶ�� 'RD' Ϊ�״�ԭʼ���ݣ�'GD'Ϊ�״��������ݵȵȣ���4�� (0-3)
	float VersionNo;//��ʾ���ݸ�ʽ�İ汾�š�4��(4-7)
	long int FileHeaderLength;// ��ʾ�ļ�ͷ�ĳ��ȡ�4��(8-11)
}tagFileIdentification;

//struct LAYERPARAM LayerInfo �������Ϣ,���32��
//PPI��RHI��Ӧ��ʼ�������Ϣ����ɨ�е�ÿһ������Ӧ���������
typedef struct LAYERPARAM  //��35��
{
	unsigned char DataType;  /*  ����۲�Ҫ��  (0)
							 1=��ǿ��
							 2=��Ҫ�� ��PRF
							 3=��Ҫ�� ˫PRF
							 4=˫��ƫ��
							 5=˫��ƫ�������
							 6=˫��������ͬ���ߣ�
							 7=˫�������������ߣ�
							 8=V+W ��PRF
							 9=V+W ˫PRF
							 */
	unsigned char Ambiguousp;/* �������ٶ�ģ��״̬  (1)
							 0 = �����ٶ�ģ��״̬
							 1 = ������ٶ�ģ��
							 2 = ˫T���ٶ�ģ��
							 3 = ��ʽ���ٶ�ģ��
							 4 = ˫T + ������ٶ�ģ��
							 5 = ��ʽ + ������ٶ�ģ��
							 6 = ˫PPI���ٶ�ģ��
							 9 = ������ʽ
							 */
	unsigned short Arotate;// ��������ת��,������λΪ0.01��/�룬��ɨ�跽ʽΪRHI��PPIʱ
	//ֻ�ڵ�һ��Ԫ������д������Ԫ��Ϊ0  (2-3)
	unsigned short PRF1;//  �����һ�����ظ�Ƶ��,������λ:1/10Hz   (4-5)
	unsigned short PRF2;//  ����ڶ������ظ�Ƶ��,������λ:1/10Hz   (6-7)
	unsigned short PulseW;//�����������,������λΪ��1/100��΢��  (8-9)
	unsigned short MaxV;  //��������ɲ��ٶ�,������λΪ����/��	(10-11)
	unsigned short MaxL;  //��������ɲ����,��10��Ϊ������λ	(12-13)
	unsigned short ZbinWidth;  //  ����ǿ�����ݵĿⳤ,��1/10��Ϊ������λ(14-15)
	unsigned short VBinWidth;  //  �����ٶ����ݵĿⳤ,��1/10��Ϊ������λ(16-17)
	unsigned short WBinWidth;  //  �����׿����ݵĿⳤ,��1/10��Ϊ������λ(18-19)
	unsigned short ZbinNumber; //  ����ɨ��ǿ�Ⱦ���Ŀ���	(20-21)
	unsigned short VbinNumber; //  ����ɨ���ٶȾ���Ŀ���	(22-23)
	unsigned short WBinNumber;//   ����ɨ���׿���Ŀ���	(24-25)
	unsigned short RecordNumber;// ����ɨ�辶�����			(26-27)
	short SwpAngles;             /*���������,������λΪ1/100��,��ɨ�跽ʽ
								 ΪRHI����������飬��ɨ�跽ʽΪPPIʱ��
								 ��һ��Ԫ��Ϊ��PPIʱ�����ǣ�������λΪ
								 1/100�ȣ�����Ԫ����д-32768  (28-29)
								 */
	char DataForm; /* ���㾶���е��������з�ʽ:  (30)
				   11��Ҫ�����У� CorZ
				   12��Ҫ�����У� UnZ
				   13��Ҫ�����У� V
				   14��Ҫ�����У� W
				   21��Ҫ�����У�CorZ+ UnZ
				   22��Ҫ�����У�CorZ+ V+ W
				   23��Ҫ�����У�UnZ+ V+ W
				   24��Ҫ�����У�CorZ+UnZ+ V+ W
				   25��Ҫ�����У� V+ W
				   4x˫ƫ��Ҫ������ģʽ
				   6x˫ƫ������հ�Ҫ������ģʽ
				   8x˫������Ҫ������ģʽ
				   */
	unsigned long int DBegin;// �������ݼ�¼��ʼλ��(�ֽ���) (31-34)
}tagLayerInfo;

//ǿ�ȿⳤ�б仯���ݽṹ struct BINPARAM  BinRange1��8���ֽڣ��������£�
typedef struct BINPARAM1 //��8��
{
	short Code; //  ǿ�ȱ�ⳤ�ṹ����				(0-1)
	short Begin;//  ��ʼ��ľ��룬��10��Ϊ������λ	(2-3)
	short End;  //  ������ľ��룬��10��Ϊ������λ	(4-5)
	short BinLength;// �ⳤ����1/10��Ϊ������λ		(6-7)
}tagBinRange1;

//�ٶȿⳤ�б仯���ݽṹ struct BINPARAM  BinRange2��8���ֽڣ��������£�
typedef struct BINPARAM2 //��8��
{
	short Code;//  �ٶȱ�ⳤ�ṹ����			 (0-1)
	short Begin;// ��ʼ��ľ��룬��10��Ϊ������λ(2-3)
	short End;// ������ľ��룬��10��Ϊ������λ	 (4-5)
	short BinLength;//�ⳤ����1/10��Ϊ������λ	 (6-7)
}tagBinRange2;

//�׿�ⳤ�б仯���ݽṹ struct BINPARAM  BinRange3��8���ֽڣ���������:
typedef struct BINPARAM3 //��8��
{
	short Code;// �׿��ⳤ�ṹ����              (0-1)
	short Begin;// ��ʼ��ľ��룬��10��Ϊ������λ (2-3)
	short End; // ������ľ��룬��10��Ϊ������λ  (4-5)
	short BinLength;// �ⳤ����1/10��Ϊ������λ	  (6-7)

}tagBinRange3;

//�ļ�ͷ-վַ������,վַ�������struct RADARSITE  SiteInfo����168���ֽڣ�
typedef struct _RADAR_SITE_INFO //��168��
{
		char Country[30];       	// ���������ı���ʽ���� (12-41)
		char Province[20];       	// ʡ�����ı���ʽ����   (42-61)
		char Station[40];          	// վ�����ı���ʽ����	(62-101)
		char StationNumber[10];     // ��վ�ţ��ı���ʽ����	(102-111)
		char RadarType[20];         // �״��ͺţ��ı���ʽ����		      (112-131)
		char Longitude[16];        	// �������ھ��ȣ��ı���ʽ���� ��д��ʽ����E115��32'12" (132-147)
		char Latitude[16];          // ��������γ�ȣ��ı���ʽ���� ��д��ʽ����N35��30'15"	(148-163)
		long int LongitudeValue;    // �������ھ��ȵ���ֵ����1/1000��Ϊ������λ	������E��Ϊ����������W��Ϊ��	  (164-167)
		long int LatitudeValue;     // ��������γ�ȵ���ֵ����1/1000��Ϊ������λ  ��γ��N��Ϊ������γ��S��Ϊ��	  (168-171)
		long int Height;        	// ���ߺ��θ߶��Ժ���Ϊ������λ	  (172-175)
		short MaxAngle;             // ��վ���ܵ�������ڵ����ǣ���1/100��Ϊ������λ						          (176-177)
		short OptiAngle;            // ��վ����ѹ۲����ǣ�����ز�ǿ��<10dBZ������1/100��Ϊ������λ			      (178-179)

}tagSiteInfo;

//ͷ�ļ�-�״����ܲ�����,���ܲ���struct RADARPERFORMANCEPARAM  PerformanceInfo����36���ֽڣ�
typedef struct _RADAR_PERFORMANCE_PARAM //��36��
{
		long int AntennaG;          	//����������0.001dBΪ������λ	  (180-183)
		unsigned short VerBeamW;    // ��ֱ���������1/100��Ϊ������λ  (184-185)
		unsigned short HorBeamW;    //ˮƽ���������1/100��Ϊ������λ  (186-187)
		unsigned char Polarizations;//ƫ��״��	(188)
									/*0=ˮƽ
									-1=��ֱ
									-2=˫��ƫ��
									-3=Բƫ��
									-4=����
									*/
		unsigned short SideLobe;   // ��һ�԰꣬��0.01dBΪ������λ	  (189-190)
		long int Power;            // �״������ֵ���ʣ�����Ϊ������λ  (191-194)
		long int WaveLength;       // ��������΢��Ϊ������λ			  (195-198)
		unsigned short LogA;       // �������ջ���̬��Χ����0.01dBΪ������λ(199-200)
		unsigned short LineA;      // ���Խ��ջ���̬��Χ����0.01dBΪ������λ(201-202)
		unsigned short AGCP;       // AGC�ӳ�������΢��Ϊ������λ		      (203-204)
		unsigned short LogMinPower;//  �������ջ���С�ɲ⹦�ʣ�������λΪ 0.01dBm							      (205-206)
		unsigned short LineMinPower;// ���Խ��ջ���С�ɲ⹦�ʣ�������λΪ 0.01dBm							      (207-208)
		unsigned char ClutterT;    // �Ӳ�������ֵ��������λΪ0.01dB	 (209)
		unsigned char VelocityP;   /* �ٶȴ���ʽ	 (210)
									-0=���ٶȴ���
									-1=PPP
									-2=FFT
								   */
		unsigned char FilterP;     /* �����Ӳ�������ʽ	 (211)
									0=�޵����Ӳ�����
									1=�����Ӳ�ͼ�۳���
									2=�����Ӳ�ͼ+�˲�������
									3=�˲�������
									4=�׷�������
									5=��������
									*/
		unsigned char NoiseT;      // ����������ֵ��0-255��	 (212)
		unsigned char SQIT;        // SQI��ֵ����0.01Ϊ������λ   (213)
		unsigned char IntensityC;  /* RVPǿ��ֵ������õ�ͨ��	 (214)
									1=����ͨ��
									2=����ͨ��
									*/
		unsigned char IntensityR;  /* ǿ�ȹ����Ƿ�����˾��붩��  (215)
									0=��
									1=�ѽ����˾��붩��
									*/
}tagPerformanceInfo;

//ͷ�ļ�-�״�۲������,�۲����struct RADAROBSERVATIONPARAM  ObservationInfo����1282���ֽڣ�
typedef struct _RADAR_OBSERVATION_PARAM //��1282��
{
	    unsigned char SType;    /* ɨ�跽ʽ	(216)
								1=RHI
								10=PPI
								1XX=VOL��XXΪɨ�����
								*/
		unsigned short SYear;   // �۲��¼��ʼʱ����꣨2000 -   ��(217-218)
		unsigned char SMonth;	// �۲��¼��ʼʱ����£�1-12��	 (219)
		unsigned char SDay;		// �۲��¼��ʼʱ����գ�1-31��	 (220)
		unsigned char SHour;	// �۲��¼��ʼʱ���ʱ��00-23�� (221)
		unsigned char SMinute;	// �۲��¼��ʼʱ��ķ֣�00-59�� (222)
		unsigned char SSecond;	// �۲��¼��ʼʱ����루00-59�� (223)
		unsigned char TimeP; 			/*ʱ����Դ	(224)
										0=�����ʱ�ӣ���һ����δ���ж�ʱ
										1=�����ʱ�ӣ�һ�����ѽ��ж�ʱ
										2=GPS
										3=����
										*/
		unsigned long int SMillisecond; 	//���С��λ��������λ΢�룩(225-228)
		unsigned char Calibration; 		/*��У״̬	(229)
										0=�ޱ�У
										1=�Զ���У
										2=һ�������˹���У
										3=һ�����˹���У
										�����벻��
										*/
		unsigned char IntensityI; 		// ǿ�Ȼ��ִ�����32-128��(230)
		unsigned char VelocityP;		// �ٶȴ���������31-255������������1��(231)
		unsigned short ZStartBin; 		// ǿ����Ч���ݿ�ʼ����	(232-233)
		unsigned short VStartBin;  		// �ٶ���Ч���ݿ�ʼ����	(234-235)
		unsigned short WStartBin;  		// �׿���Ч���ݿ�ʼ����	(236-237)
	    tagLayerInfo LayerInfo[32];// ��������ݽṹ(��Ȧɨ��״̬����) (238-1357)
		unsigned short RHIA; 	        // ��RHIʱ�����ڷ�λ�ǣ�������λΪ1/100�ȣ���PPI������ɨ��ʱΪ65535	 (1358-1359)
		short RHIL;	// ��RHIʱ��������ǣ�������λΪ1/100�ȣ�������ɨ��ʱΪ-32768  (1360-1361)
		short RHIH;	// ��RHIʱ��������ǣ�������λΪ1/100�ȣ�������ɨ��ʱΪ-32768  (1362-1363)
		unsigned short EYear;// �۲��¼����ʱ����꣨2000 -   ��(1364-1365)
		unsigned char EMonth;// �۲��¼����ʱ����£�1-12��(1366)
		unsigned char EDay;	// �۲��¼����ʱ����գ�1-31��	(1367)
		unsigned char EHour;// �۲��¼����ʱ���ʱ��00-23��(1368)
		unsigned char EMinute;// �۲��¼����ʱ��ķ֣�00-59�� (1369)
		unsigned char ESecond;// �۲��¼����ʱ����루00-59�� (1370)
		unsigned char ETenth;// �۲��¼����ʱ���1/100�루00-99��(1371)
		unsigned short ZBinByte; //ԭʼǿ�������пⳤ�ޱ仯��0 (1372-1373)	ԭʼǿ�������пⳤ�б仯��ռ���ֽ���
	    tagBinRange1  BinRange1[5]; // 5��8�ֽ�(ǿ�ȿⳤ�ޱ仯Ϊ���ֽ�)(1374-1413)
		unsigned short VBinByte; // ԭʼ�ٶ������пⳤ�ޱ仯��0 	(1414-1415)		ԭʼ�ٶ������пⳤ�б仯��ռ���ֽ���
	    tagBinRange2  BinRange2[5]; // 5��8�ֽڣ��ٶȿⳤ�ޱ仯Ϊ���ֽڣ�(1416-1455)
	    unsigned short WBinByte; // ԭʼ�׿������пⳤ�ޱ仯��0 	(1456-1457)	ԭʼ�׿������пⳤ�б仯��ռ���ֽ���
	    tagBinRange3  BinRange3[5];// 5��8�ֽڣ��׿�ⳤ�ޱ仯Ϊ���ֽڣ�(1458-1497)

}tagObservationInfo;

//�ļ�ͷ-������Ϣ��,������Ϣ����struct RADAROTHERINFORMATION  OtherInfo����562���ֽ�)
typedef struct _RADADR_OTHER_INFORMATION //��562��
{
	char StationID[2];// ̨վ����	   (1498-1499)
	char Spare[560];  // �����ֽ�560�� (1500-2059)

}tagOtherInfo;

//���ݼ�¼��
//nΪ�ļ�ͷ����ɨ����Ӧ�㣨��PPI��RHI��ʼ�㣩��Ӧ��ǿ�ȡ��ٶȡ��׿�ĸ�������Ŀ�����
typedef struct _RADAR_DATA_RECORD //��???��
{
		short Elev;	//	���ǣ�������λ1/100��
		unsigned short Az;//��λ��������λ1/100��
		unsigned char Hh;//	ʱ
		unsigned char Mm;//	��
		unsigned char Ss;//	��
		unsigned long int Min;// ���С����������λΪ΢�룩
		//unsigned char CorZ[n];// ���������Ӳ�������dBZֵ=(CorZ-64)/2   �޻ز�ʱ��Ϊ��
		//unsigned char UnZ[n]; // �����������Ӳ�������dBZֵ=(UnZ-64)/2  �޻ز�ʱ��Ϊ��
		//unsigned char V[n]; //   �ٶ�ֵ��������λΪ���ɲ��ٶȵ�1/127 
							//   ��ֵ��ʾԶ���״���ٶȣ���ֵ��ʾ�����״���ٶ� �޻ز�ʱ��-128
		
		//unsigned char W[n]; //   �׿�ֵ��������λΪ���ɲ��ٶȵ�1/512	�޻ز�ʱ��Ϊ��
}tagRecordInfo;



//�״��ļ�ͷ���ļ���ʶ���ļ�ͷ��վַ�Ρ����ܲ����Ρ��۲�����Σ�������������
typedef struct _RADAR_FILE_HEAD
{
	tagFileIdentification  FileIdentification;//�ļ���ʶ
	tagSiteInfo            SiteInfo;//վַ������
	tagPerformanceInfo     PerformaceInfo;//���ܲ�����
	tagObservationInfo     ObservationInfo;//�۲������
	tagOtherInfo           OtherInfo;//��������
}tagRadarFileHead;

#pragma pack(pop)
#endif