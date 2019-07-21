#ifndef radialhead_h__20081004
#define radialhead_h__20081004


#pragma pack(1)		// one byte alignment

typedef struct _SEALS_LineDataBlock  //346_S径向数据头【20】
{
	long int Longtitude;    //!< 经度【4】 单位 0.01 秒
	long int Latitude;		//!< 维度【4】 单位 0.01 秒
	float Vs;				//!< 纵摇【4】 单位 0.01 度
	float Hs;				//!< 横摇【4】 单位 0.01 度
	unsigned short Course;  //!< 航向【2】 单位 0.01 度
	short Nv;		//!< 舰速【2】 单位 0.01 米/秒
}tagLDB_Seals;

//! FL径向数据头
typedef struct _FLX_LDB  // 【28】
{
	unsigned short		spa9B;		//!< space 【2】
	unsigned short		spa9A;		//!< space 【2】
	unsigned short		spa10B;		//!< space 【2】
	unsigned short		spa10A;		//!< space 【2】
	unsigned short		spa11B;		//!< space 【2】
	unsigned short		spa11A;		//!< space 【2】
	unsigned short		spa12B;		//!< space 【2】
	unsigned short		spa12A;		//!< space 【2】
	unsigned short		spa13B;		//!< space 【2】
	unsigned short		spa13A;		//!< space 【2】

	unsigned short		ipqPowLinFilter;	//!< IPOWER/I Power Lin Filtered	:14:16-31 【2】
	unsigned short		qpqPowLinFilter;	//!< QPOWER/Q Power Lin Filtered	:14:00-15 【2】
	unsigned short		iByPowLinNoFil;	//!< IBYPPOWER/I ByPower Lin No Filter :15:16-31 【2】
	unsigned short		qByPowLinNoFil;	//!< QBYPPOWER/Q ByPower Lin No Filter :15:00-15 【2】
}tagLDB_FLX;

/*! \brief 径向数据头
	
	记录每个径向数据的径向信息
*/
typedef struct _BASE_DATA_HEAD  // 【64】
{
	unsigned long		spa0_0:11;		//!< space	:0:16-26, =0x7ff status is standby 【b:11】
	unsigned long		sigProStatu:5;	//!< SIGNAL PROCESSOR STATUS	:0:27-31 【b:5】

	unsigned long		radNumbe:10;	//!< RADIAL NUMBER	:0:0-9		【b:10】
	unsigned long		lrf:1;			//!< LRF				:0:10	【b:1】
	unsigned long		spa0_11:5;		//!< space			:0:11-15	【b:5】

	unsigned long		aziAngle:16;	//!< Azimuth Angle	:1:16-31	【2】
	unsigned long		EleAngle:16;	//!< Elevation		:1:0-15		【2】

	unsigned long		prtInter1:16;	//!< PRT INTERVAL1	:2:16-31	【2】
	unsigned long		prtInter2:16;	// PRT INTERVAL2	:2:0-15	【2】

	unsigned long		curRate:16;		//!< Current antenna Rate	:3:16-31	【2】
	unsigned long		spoBlaFlag:16;	//!< SPOT BLANKING FLAG	:3:0-15	【2】

	unsigned long		endAroTesWord;	//!< END AROUND TEST WORD	:4:0-31	【4】
	// 5B -7B word
	unsigned long		year:8;			//!< year	【1】
	unsigned long       month:8;        //!< month	【1】
	unsigned long		day:8;			//!< day	【1】
	unsigned long		hour:8;			//!< hour	【1】
	unsigned long		min:8;		//!< minute		【1】
	unsigned long		sec:8;		//!< second		【1】
	unsigned long		EleStart:16;		//!< space	【2】
	unsigned long		EleStop:16;		//!< space	【2】
	unsigned long		recIntTesData:8;	//!< Receiver Interface Test Data	:7:0-7	【1】 
	unsigned long		flags:8;    /*! 标志码 :8:8-15	【1】
									- 1. 表示扫描开始的第一个径向；
									- 2. 用于立体扫描中，下一个PPI的开始和结束的径向，其他径向填0；
									- 3. 同2；
									- 4. 表示扫描结束；
									*/
	unsigned long		id;             //!< the id of radial's parents PPI :8:0-31	【4】


	union  // 【28】
	{
		tagLDB_Seals LDB_Seals;
		tagLDB_FLX LDB_FLX;
	};
}BASE_DATA_HEAD,_tagBaseDataHead,tagRadialHead;

#pragma pack()


#endif // radialhead_h__

