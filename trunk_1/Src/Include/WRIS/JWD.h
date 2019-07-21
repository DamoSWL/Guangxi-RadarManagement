
#ifndef __JWD_AND_HELPER_20051020
#define __JWD_AND_HELPER_20051020

#include <math.h>


//static const double PI = 3.14159265;
static double Rc = 6378137;  // ����뾶
static double Rj = 6356725;  // ���뾶	

namespace CDYW
{

	class JWD
	{
	public:
		double m_LoDeg, m_LoMin, m_LoSec;  // longtitude ����
		double m_LaDeg, m_LaMin, m_LaSec;
		double m_Longitude, m_Latitude;
		double m_RadLo, m_RadLa;
		double Ec;
		double Ed;

	public:
		// ���캯��
		JWD(double loDeg, double loMin, double loSec, double laDeg, double laMin, double laSec)
		{
			m_LoDeg=loDeg; m_LoMin=loMin; m_LoSec=loSec; m_LaDeg=laDeg; m_LaMin=laMin; m_LaSec=laSec;
			m_Longitude = m_LoDeg + m_LoMin / 60 + m_LoSec / 3600;
			m_Latitude = m_LaDeg + m_LaMin / 60 + m_LaSec / 3600;
			m_RadLo  = m_Longitude * M_PI / 180.;
			m_RadLa  = m_Latitude * M_PI / 180.;
			Ec = Rj + (Rc - Rj) * (90.- m_Latitude) / 90.;
			Ed = Ec * cos(m_RadLa);
		}

		//��
		JWD(double longitude, double latitude)
		{
			double dlongitude = longitude;///360000;
			double dlatitude = latitude;///360000;

			m_LoDeg = int(dlongitude);
			m_LoMin = int((dlongitude - m_LoDeg)*60);
			m_LoSec = (dlongitude - m_LoDeg - m_LoMin/60.)*3600;

			if ( m_LoSec > 59 )
			{
				m_LoSec = 0;
				m_LoMin += 1;
			}

			m_LaDeg = int(dlatitude);
			m_LaMin = int((dlatitude - m_LaDeg)*60);
			m_LaSec = (dlatitude - m_LaDeg - m_LaMin/60.)*3600;

			if ( m_LaSec > 59 )
			{
				m_LaSec = 0;
				m_LaMin += 1;
			}

			m_Longitude = longitude;
			m_Latitude = latitude;
			m_RadLo = longitude * M_PI/180.;
			m_RadLa = latitude * M_PI/180.;
			Ec = Rj + (Rc - Rj) * (90.-m_Latitude) / 90.;
			Ed = Ec * cos(m_RadLa);
		}
	};

	class CJWDHelper
	{
	public:
		CJWDHelper() {};
		~CJWDHelper() {};

		//! �����A �� ��B�ľ�γ�ȣ������ǵľ���͵�B����ڵ�A�ķ�λ
		/*! 
		* \param A A�㾭γ��
		* \param B B�㾭γ��
		* \param angle B�����A�ķ�λ, ����Ҫ���ظ�ֵ��������Ϊ��
		* \return A��B��ľ���
		*/
		static double distance(const JWD &A, const JWD &B, double *angle)
		{
			double dx = (B.m_RadLo - A.m_RadLo) * A.Ed;
			double dy = (B.m_RadLa - A.m_RadLa) * A.Ec;
			double out = sqrt(dx * dx + dy * dy);

			if( angle != NULL)
			{
				*angle = atan(fabs(dx/dy))*180./M_PI;
				// �ж�����
				double dLo = B.m_Longitude - A.m_Longitude;
				double dLa = B.m_Latitude - A.m_Latitude;

				if(dLo > 0 && dLa <= 0) {
					*angle = (90. - *angle) + 90.;
				}
				else if(dLo <= 0 && dLa < 0) {
					*angle = *angle + 180.;
				}
				else if(dLo < 0 && dLa >= 0) {
					*angle = (90. - *angle) + 270;
				}
			}

			return out;
		}

		//! �����A �� ��B�ľ�γ�ȣ������ǵľ���͵�B����ڵ�A�ķ�λ
		/*! 
		* \param longitude1 A�㾭��
		* \param latitude1 A��γ��
		* \param longitude2 B�㾭��
		* \param latitude2 B��γ��
		* \param angle B�����A�ķ�λ, ����Ҫ���ظ�ֵ��������Ϊ��
		* \return A��B��ľ���
		*/
		inline static double distance(
			const double &longitude1, const double &latitude1,
			const double &longitude2, const double &latitude2, 
			double *angle)
		{
			JWD A(longitude1,latitude1);
			JWD B(longitude2,latitude2);

			return distance(A, B, angle);
		}

		//! ��֪��A��γ�ȣ�����B���A��ľ��룬�ͷ�λ����B��ľ�γ��
		/*!
		* \param A ��֪��A
		* \param distance B�㵽A��ľ���(km) 
		* \param angle B�������A��ķ�λ
		* \return B��ľ�γ������
		*/
		static JWD GetJWDB(const JWD &A, const double &distance, const double &angle)
		{	
			double dx = distance*1000 * sin(angle * M_PI /180.);
			double dy = distance*1000 * cos(angle * M_PI /180.);

			//double dx = (B.m_RadLo - A.m_RadLo) * A.Ed;
			//double dy = (B.m_RadLa - A.m_RadLa) * A.Ec;

			double BJD = (dx/A.Ed + A.m_RadLo) * 180./M_PI;
			double BWD = (dy/A.Ec + A.m_RadLa) * 180./M_PI;
			JWD B(BJD, BWD);
			return B; 
		}

		//! ��֪��A��γ�ȣ�����B���A��ľ��룬�ͷ�λ����B��ľ�γ��
		/*!
		* \param longitude ��֪��A����
		* \param latitude ��֪��Aγ��
		* \param distance B�㵽A��ľ��� 
		* \param angle B�������A��ķ�λ
		* \return B��ľ�γ������
		*/
		static JWD GetJWDB(const double &longitude, const double &latitude, const double &distance, const double &angle)
		{	
			JWD A(longitude,latitude);
			return GetJWDB(A, distance, angle);
		}

	};
}

#endif