#ifndef _FORMAT_DATA_
#define _FORMAT_DATA_

///////8位加权格式///////////////////////////
////浮点数据到8位UCHAR转换公式
//		宏定义			简化公式				名称：原型公式					取值范围			位权值
#define SET_REF_VAL8(V)	((V)*2+66)				//反射率因子：(((V)+32)*2 + 2 )	// -32<=V<=94.5		LSB=0.5(dBz)
#define SET_VEL_VAL8(V)	((V)*2+129)				//基本速度：(((V)+63.5)*2 + 2 )	// -63.5<=V<=63.0	LSB=0.5(m/s)
#define SET_WID_VAL8(V)	((V)*2+129)				//基本谱宽						//	0<=V<=63		LSB=0.5(m/s)
#define SET_ZDR_VAL8(V)	((V)*10+50)				//差分反射率因子(((V)+5)*10)	// -3<=V<=6			LSB=0.1(dB)
#define SET_RHV_VAL8(V) ((V)*100+5)				//相关系数						//0.00<=V<=1.00		LSB=0.01(无量纲)
#define SET_KDP_VAL8(V)	((V)*20+60)				//差分相移因子(比相差)(((V)+3)*20)	// -2.0<=V<=5.00	LSB=0.05(deg/km)
#define SET_PDP_VAL8(V)	((V)/1.5+2)			//差分相位						// 0 <=V <360		LSB=1.5(度)   

#define SET_ARD_VAL(V)	((V)*4.0+82.0)			//方位涡度						//-20<=V<=43		LSB=0.25 (E-4/s)	
#define SET_CS_VAL(V)	((V)+40.0)				//组合切变						//0<=V<=215			LSB=1(E-4/s)
#define SET_ETEB_VAL(V)	((V)*0.01+2)			//回波顶/底高((V)/100.0+2)		//0<=V<=25400		LSB=100(m)
#define SET_LTA_VAL(V)	((V)*8.0+2)				//分层组合湍流平均值			//0<=V<=31.6		LSB=0.125 (m^2/s^3)	
#define SET_RZ_VAL(V)	((V)*2.0+2)				//雨强							//0<V<=126.5		LSB=0.5(m^2/h)
#define SET_OHP_VAL(V)	((V)/1.27+2)			//小时降雨量			//0<V<=253			LSB=1(m^2/km^2)
#define SET_RVD_VAL(V)	((V)*4.0+82.0)			//速度径向散度					//-20<=V<=43.3		LSB= (E-4/s)
#define SET_VAD_VAL(V)	((V)*2+66)				//速度方位显示(((V)+32)*2 + 2 )	//-32<=V<=94.5		LSB=0.5					XXXXXXXXXXXXXX
#define SET_VIL_VAL(V)	((V)*2+56)				//垂直液态水含量				//0<V<=99.5		LSB=0.5(kg/m^2)
#define SET_VWP_VAL(V)	((V)+5.5)				//垂直风廓线(((V)+0.5)+5 )		//0<=V<=31			LSB=1(m/s)
////END浮点数据到8位UCHAR转换公式

////8位UCHAR到浮点值转换公式
#define GET_REF_VAL8(N)	((N)*0.5-33)			//((N)-2)*0.5-32.0f		//2<=N<=255
#define GET_VEL_VAL8(N)	((N)*0.5-64.5)			//((N)-2)*0.5-63.5f		//2<=N<=255
#define GET_WID_VAL8(N)	((N)*0.5-64.5)			//((N)-129.0f)/2		//129<=N=255
#define GET_ZDR_VAL8(N)	((N)*0.1-5)				//((N)/10.0-5.0)		// ‘1’模糊；N<20无效；20<= N <=110有效范围 
#define GET_RHV_VAL8(N)	((N)*0.01-0.05)			//((N)/100.0-0.05)		// ‘1’模糊；N<5无效；5<= N <=105
#define GET_KDP_VAL8(N)	((N)*0.05-3)			//((N)/20.0-3.0)		// ‘1’模糊；N<20无效；20<= N <=160
#define GET_PDP_VAL8(N)	((N)*1.5-3)			//						// ‘1’模糊；2<= N <242

#define GET_VIL_VAL(N)	((N)*0.5-28)			//(((N)-56.0)/2.0 )		//56<=N<=255

////END8位UCHAR到浮点值转换公式


#define MAXVALUE8	255
#define MINVALUE8	2
/////////END 8位加权格式///////////////////////

//16位加权格式///////////////////////////
#define SET_REF_VAL16(V) (V*100+32768)		//	
#define GET_REF_VAL16(N) ((N-32768)/100.0)	//LSB=0.01dBz；	“0”无效；“1”-327.67dBZ；	“32768”0dBZ；	“65534”+327.66dBZ；"65535"保留

#define SET_VEL_VAL16(V) (V*100+32768)		//
#define GET_VEL_VAL16(N) ((N-32768)/100.0)	//LSB=0.01m/s；	“0”无效；“1”-327.67m/s；	“32768”0m/s；	“65534”+327.660m/s；"65535"保留

#define SET_WID_VAL16(V) (V*100)			//		
#define GET_WID_VAL16(N) (N/100.0)			//LSB=0.01m/s；	“0”无效；“1”0.01m/s；						“65534”655.34m/s；"65535"保留

#define SET_ZDR_VAL16(V) (V*100+32768)		//
#define GET_ZDR_VAL16(N) ((N-32768)/100.0)	//LSB=0.01dB；	“0”无效；“1”-327.67dBZ；	“32768”0dBZ；	“65534”+327.66dBZ；"65535"保留

#define SET_KDP_VAL16(V) (V*100+32768)		//
#define GET_KDP_VAL16(N) ((N-32768)/100.0)	//LSB=0.01；	“0”无效；“1”-327.67°/km；“32768”0°/km；“65534”+327.66°/km；"65535"保留

#define SET_RHV_VAL16(V) (V*65533+1)		//
#define GET_RHV_VAL16(N) ((N-1)/65533.0)		//LSB=0.0000153；“0”无效；“1”0；			“65534”1；"65535"保留

#define SET_PDP_VAL16(V) (V*65534.0/360.0+2)	//
#define GET_PDP_VAL16(N) (360*(N-2)/65534.0)	//LSB=0.0055度；“0”"1"无效；“2”0.000°；		“65535”359.995°；

#define SET_SQI_VAL16(V) (V*65533+1)		//
#define GET_SQI_VAL16(N) ((N-1)/65533.0)		//LSB=0.0000153；“0”无效；“1”0；			“65534”1；"65535"保留

#define PMAX_REF		94.5
#define PMIN_REF		-32
#define PMAX_VEL		63.5
#define PMIN_VEL		-63.5
#define PMAX_WID		63
#define PMIN_WID		0

#define PMAX_RHV		1
#define PMIN_RVH		0
#define PMAX_KDP		5
#define PMIN_KDP		-2
#define PMAX_PDP		360
#define PMIN_PDP		0
#define PMAX_ZDR		6
#define PMIN_ZDR		-3

#define MAXVALUE16		65535
#define MINVALUE16		2
#define OVERLAIDVALUE	1
#define VALIDVALUE		0
//END 16位加权格式///////////////////////

#endif