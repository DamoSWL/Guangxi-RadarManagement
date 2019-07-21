#ifndef _DATTwo_
#define _DATTwo_

//雷达原始数据格式（VOL扫描数据格式、PPI和RHI扫描数据格式）
#pragma pack(push, 1)
//文件标识
typedef struct _RADAR_FILE_IDENTIFICATION  //【12】
{
	char FileID[4]; //雷达数据标识（原始数据标识符 'RD' 为雷达原始数据，'GD'为雷达衍生数据等等）【4】 (0-3)
	float VersionNo;//表示数据格式的版本号【4】(4-7)
	long int FileHeaderLength;// 表示文件头的长度【4】(8-11)
}tagFileIdentification;

//struct LAYERPARAM LayerInfo 层参数信息,最大32层
//PPI和RHI对应开始层参数信息，体扫中的每一层由相应层参数描述
typedef struct LAYERPARAM  //【35】
{
	unsigned char DataType;  /*  本层观测要素  (0)
							 1=单强度
							 2=三要素 单PRF
							 3=三要素 双PRF
							 4=双线偏振
							 5=双线偏振多普勒
							 6=双波长（不同天线）
							 7=双波长（共用天线）
							 8=V+W 单PRF
							 9=V+W 双PRF
							 */
	unsigned char Ambiguousp;/* 本层退速度模糊状态  (1)
							 0 = 无退速度模糊状态
							 1 = 软件退速度模糊
							 2 = 双T退速度模糊
							 3 = 批式退速度模糊
							 4 = 双T + 软件退速度模糊
							 5 = 批式 + 软件退速度模糊
							 6 = 双PPI退速度模糊
							 9 = 其他方式
							 */
	unsigned short Arotate;// 本层天线转速,计数单位为0.01度/秒，当扫描方式为RHI或PPI时
	//只在第一个元素中填写，其他元素为0  (2-3)
	unsigned short PRF1;//  本层第一脉冲重复频率,计数单位:1/10Hz   (4-5)
	unsigned short PRF2;//  本层第二脉冲重复频率,计数单位:1/10Hz   (6-7)
	unsigned short PulseW;//本层的脉冲宽度,计数单位为（1/100）微秒  (8-9)
	unsigned short MaxV;  //本层的最大可测速度,计数单位为厘米/秒	(10-11)
	unsigned short MaxL;  //本层的最大可测距离,以10米为计数单位	(12-13)
	unsigned short ZbinWidth;  //  本层强度数据的库长,以1/10米为计数单位(14-15)
	unsigned short VBinWidth;  //  本层速度数据的库长,以1/10米为计数单位(16-17)
	unsigned short WBinWidth;  //  本层谱宽数据的库长,以1/10米为计数单位(18-19)
	unsigned short ZbinNumber; //  本层扫描强度径向的库数	(20-21)
	unsigned short VbinNumber; //  本层扫描速度径向的库数	(22-23)
	unsigned short WBinNumber;//   本层扫描谱宽径向的库数	(24-25)
	unsigned short RecordNumber;// 本层扫描径向个数			(26-27)
	short SwpAngles;             /*本层的仰角,计数单位为1/100度,当扫描方式
								 为RHI，不填此数组，当扫描方式为PPI时，
								 第一个元素为做PPI时的仰角，计数单位为
								 1/100度，其他元素填写-32768  (28-29)
								 */
	char DataForm; /* 本层径向中的数据排列方式:  (30)
				   11单要素排列： CorZ
				   12单要素排列： UnZ
				   13单要素排列： V
				   14单要素排列： W
				   21按要素排列：CorZ+ UnZ
				   22按要素排列：CorZ+ V+ W
				   23按要素排列：UnZ+ V+ W
				   24按要素排列：CorZ+UnZ+ V+ W
				   25按要素排列： V+ W
				   4x双偏振按要素排列模式
				   6x双偏振多普勒按要素排列模式
				   8x双波长按要素排列模式
				   */
	unsigned long int DBegin;// 本层数据记录开始位置(字节数) (31-34)
}tagLayerInfo;

//强度库长有变化数据结构 struct BINPARAM  BinRange1（8个字节）定义如下：
typedef struct BINPARAM1 //【8】
{
	short Code; //  强度变库长结构代码				(0-1)
	short Begin;//  开始库的距离，以10米为计数单位	(2-3)
	short End;  //  结束库的距离，以10米为计数单位	(4-5)
	short BinLength;// 库长，以1/10米为计数单位		(6-7)
}tagBinRange1;

//速度库长有变化数据结构 struct BINPARAM  BinRange2（8个字节）定义如下：
typedef struct BINPARAM2 //【8】
{
	short Code;//  速度变库长结构代码			 (0-1)
	short Begin;// 开始库的距离，以10米为计数单位(2-3)
	short End;// 结束库的距离，以10米为计数单位	 (4-5)
	short BinLength;//库长，以1/10米为计数单位	 (6-7)
}tagBinRange2;

//谱宽库长有变化数据结构 struct BINPARAM  BinRange3（8个字节）定义如下:
typedef struct BINPARAM3 //【8】
{
	short Code;// 谱宽变库长结构代码              (0-1)
	short Begin;// 开始库的距离，以10米为计数单位 (2-3)
	short End; // 结束库的距离，以10米为计数单位  (4-5)
	short BinLength;// 库长，以1/10米为计数单位	  (6-7)

}tagBinRange3;

//文件头-站址参数段,站址基本情况struct RADARSITE  SiteInfo（共168个字节）
typedef struct _RADAR_SITE_INFO //【168】
{
		char Country[30];       	// 国家名，文本格式输入 (12-41)
		char Province[20];       	// 省名，文本格式输入   (42-61)
		char Station[40];          	// 站名，文本格式输入	(62-101)
		char StationNumber[10];     // 区站号，文本格式输入	(102-111)
		char RadarType[20];         // 雷达型号，文本格式输入		      (112-131)
		char Longitude[16];        	// 天线所在经度，文本格式输入 书写格式例：E115°32'12" (132-147)
		char Latitude[16];          // 天线所在纬度，文本格式输入 书写格式例：N35°30'15"	(148-163)
		long int LongitudeValue;    // 天线所在经度的数值，以1/1000度为计数单位	东经（E）为正，西经（W）为负	  (164-167)
		long int LatitudeValue;     // 天线所在纬度的数值，以1/1000度为计数单位  北纬（N）为正，南纬（S）为负	  (168-171)
		long int Height;        	// 天线海拔高度以毫米为计数单位	  (172-175)
		short MaxAngle;             // 测站四周地物最大遮挡仰角，以1/100度为计数单位						          (176-177)
		short OptiAngle;            // 测站的最佳观测仰角（地物回波强度<10dBZ），以1/100度为计数单位			      (178-179)

}tagSiteInfo;

//头文件-雷达性能参数段,性能参数struct RADARPERFORMANCEPARAM  PerformanceInfo（共36个字节）
typedef struct _RADAR_PERFORMANCE_PARAM //【36】
{
		long int AntennaG;          	//天线增益以0.001dB为计数单位	  (180-183)
		unsigned short VerBeamW;    // 垂直波束宽度以1/100度为计数单位  (184-185)
		unsigned short HorBeamW;    //水平波束宽度以1/100度为计数单位  (186-187)
		unsigned char Polarizations;//偏振状况	(188)
									/*0=水平
									-1=垂直
									-2=双线偏振
									-3=圆偏振
									-4=其他
									*/
		unsigned short SideLobe;   // 第一旁瓣，以0.01dB为计数单位	  (189-190)
		long int Power;            // 雷达脉冲峰值功率，以瓦为计数单位  (191-194)
		long int WaveLength;       // 波长，以微米为计数单位			  (195-198)
		unsigned short LogA;       // 对数接收机动态范围，以0.01dB为计数单位(199-200)
		unsigned short LineA;      // 线性接收机动态范围，以0.01dB为计数单位(201-202)
		unsigned short AGCP;       // AGC延迟量，以微秒为计数单位		      (203-204)
		unsigned short LogMinPower;//  对数接收机最小可测功率，计数单位为 0.01dBm							      (205-206)
		unsigned short LineMinPower;// 线性接收机最小可测功率，计数单位为 0.01dBm							      (207-208)
		unsigned char ClutterT;    // 杂波消除阈值，计数单位为0.01dB	 (209)
		unsigned char VelocityP;   /* 速度处理方式	 (210)
									-0=无速度处理
									-1=PPP
									-2=FFT
								   */
		unsigned char FilterP;     /* 地物杂波消除方式	 (211)
									0=无地物杂波消除
									1=地物杂波图扣除法
									2=地物杂波图+滤波器处理
									3=滤波器处理
									4=谱分析处理
									5=其它处理法
									*/
		unsigned char NoiseT;      // 噪声消除阈值（0-255）	 (212)
		unsigned char SQIT;        // SQI阈值，以0.01为记数单位   (213)
		unsigned char IntensityC;  /* RVP强度值估算采用的通道	 (214)
									1=对数通道
									2=线性通道
									*/
		unsigned char IntensityR;  /* 强度估算是否进行了距离订正  (215)
									0=无
									1=已进行了距离订正
									*/
}tagPerformanceInfo;

//头文件-雷达观测参数段,观测参数struct RADAROBSERVATIONPARAM  ObservationInfo（共1282个字节）
typedef struct _RADAR_OBSERVATION_PARAM //【1282】
{
	    unsigned char SType;    /* 扫描方式	(216)
								1=RHI
								10=PPI
								1XX=VOL，XX为扫描层数
								*/
		unsigned short SYear;   // 观测记录开始时间的年（2000 -   ）(217-218)
		unsigned char SMonth;	// 观测记录开始时间的月（1-12）	 (219)
		unsigned char SDay;		// 观测记录开始时间的日（1-31）	 (220)
		unsigned char SHour;	// 观测记录开始时间的时（00-23） (221)
		unsigned char SMinute;	// 观测记录开始时间的分（00-59） (222)
		unsigned char SSecond;	// 观测记录开始时间的秒（00-59） (223)
		unsigned char TimeP; 			/*时间来源	(224)
										0=计算机时钟，但一天内未进行对时
										1=计算机时钟，一天内已进行对时
										2=GPS
										3=其他
										*/
		unsigned long int SMillisecond; 	//秒的小数位（计数单位微秒）(225-228)
		unsigned char Calibration; 		/*标校状态	(229)
										0=无标校
										1=自动标校
										2=一星期内人工标校
										3=一月内人工标校
										其他码不用
										*/
		unsigned char IntensityI; 		// 强度积分次数（32-128）(230)
		unsigned char VelocityP;		// 速度处理样本（31-255）（样本数减1）(231)
		unsigned short ZStartBin; 		// 强度有效数据开始库数	(232-233)
		unsigned short VStartBin;  		// 速度有效数据开始库数	(234-235)
		unsigned short WStartBin;  		// 谱宽有效数据开始库数	(236-237)
	    tagLayerInfo LayerInfo[32];// 层参数数据结构(各圈扫描状态设置) (238-1357)
		unsigned short RHIA; 	        // 做RHI时的所在方位角，计数单位为1/100度，做PPI和立体扫描时为65535	 (1358-1359)
		short RHIL;	// 做RHI时的最低仰角，计数单位为1/100度，做其他扫描时为-32768  (1360-1361)
		short RHIH;	// 做RHI时的最高仰角，计数单位为1/100度，做其他扫描时为-32768  (1362-1363)
		unsigned short EYear;// 观测记录结束时间的年（2000 -   ）(1364-1365)
		unsigned char EMonth;// 观测记录结束时间的月（1-12）(1366)
		unsigned char EDay;	// 观测记录结束时间的日（1-31）	(1367)
		unsigned char EHour;// 观测记录结束时间的时（00-23）(1368)
		unsigned char EMinute;// 观测记录结束时间的分（00-59） (1369)
		unsigned char ESecond;// 观测记录结束时间的秒（00-59） (1370)
		unsigned char ETenth;// 观测记录结束时间的1/100秒（00-99）(1371)
		unsigned short ZBinByte; //原始强度数据中库长无变化填0 (1372-1373)	原始强度数据中库长有变化填占用字节数
	    tagBinRange1  BinRange1[5]; // 5个8字节(强度库长无变化为空字节)(1374-1413)
		unsigned short VBinByte; // 原始速度数据中库长无变化填0 	(1414-1415)		原始速度数据中库长有变化填占用字节数
	    tagBinRange2  BinRange2[5]; // 5个8字节（速度库长无变化为空字节）(1416-1455)
	    unsigned short WBinByte; // 原始谱宽数据中库长无变化填0 	(1456-1457)	原始谱宽数据中库长有变化填占用字节数
	    tagBinRange3  BinRange3[5];// 5个8字节（谱宽库长无变化为空字节）(1458-1497)

}tagObservationInfo;

//文件头-其他信息段,其它信息参数struct RADAROTHERINFORMATION  OtherInfo（共562个字节)
typedef struct _RADADR_OTHER_INFORMATION //【562】
{
	char StationID[2];// 台站代码	   (1498-1499)
	char Spare[560];  // 备用字节560个 (1500-2059)

}tagOtherInfo;

//数据记录块
//n为文件头中体扫各对应层（或PPI、RHI开始层）对应的强度、速度、谱宽的各个径向的库数。
typedef struct _RADAR_DATA_RECORD //【???】
{
		short Elev;	//	仰角，计数单位1/100度
		unsigned short Az;//方位，计数单位1/100度
		unsigned char Hh;//	时
		unsigned char Mm;//	分
		unsigned char Ss;//	秒
		unsigned long int Min;// 秒的小数（计数单位为微秒）
		//unsigned char CorZ[n];// 经过地物杂波消除的dBZ值=(CorZ-64)/2   无回波时计为零
		//unsigned char UnZ[n]; // 不经过地物杂波消除的dBZ值=(UnZ-64)/2  无回波时计为零
		//unsigned char V[n]; //   速度值，计数单位为最大可测速度的1/127 
							//   正值表示远离雷达的速度，负值表示朝向雷达的速度 无回波时计-128
		
		//unsigned char W[n]; //   谱宽值，计数单位为最大可测速度的1/512	无回波时计为零
}tagRecordInfo;



//雷达文件头（文件标识、文件头（站址段、性能参数段、观测参数段，其他参数））
typedef struct _RADAR_FILE_HEAD
{
	tagFileIdentification  FileIdentification;//文件标识
	tagSiteInfo            SiteInfo;//站址参数段
	tagPerformanceInfo     PerformaceInfo;//性能参数段
	tagObservationInfo     ObservationInfo;//观测参数段
	tagOtherInfo           OtherInfo;//其他参数
}tagRadarFileHead;

#pragma pack(pop)
#endif