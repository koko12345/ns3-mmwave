/*
 * mmwave-phy-rx-race.cc
 *
 *  Created on: Feb 2, 2015
 *      Author: macair
 */

#include <ns3/log.h>
#include "mmwave-phy-rx-trace.h"
#include <ns3/simulator.h>
#include <stdio.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MmWavePhyRxTrace");

NS_OBJECT_ENSURE_REGISTERED (MmWavePhyRxTrace);

std::ofstream MmWavePhyRxTrace::m_rxPacketTraceFile;
std::string MmWavePhyRxTrace::m_rxPacketTraceFilename;

MmWavePhyRxTrace::MmWavePhyRxTrace()
{
}

MmWavePhyRxTrace::~MmWavePhyRxTrace()
{
	if (m_rxPacketTraceFile.is_open ())
	{
		m_rxPacketTraceFile.close ();
	}
}

TypeId
MmWavePhyRxTrace::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MmWavePhyRxTrace")
    .SetParent<Object> ()
    .AddConstructor<MmWavePhyRxTrace> ()
  ;
  return tid;
}

void
MmWavePhyRxTrace::ReportCurrentCellRsrpSinrCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path,
																uint64_t imsi, SpectrumValue& sinr, SpectrumValue& power)
{
	NS_LOG_INFO ("UE"<<imsi<<"->Generate RsrpSinrTrace");
	phyStats->ReportInterferenceTrace (imsi, sinr);
	//phyStats->ReportPowerTrace (imsi, power);
}

void
MmWavePhyRxTrace::UlSinrTraceCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path,
																uint64_t imsi, SpectrumValue& sinr, SpectrumValue& power)
{
	NS_LOG_INFO ("UE"<<imsi<<"->Generate UlSinrTrace");
	uint64_t slot_count = Now().GetMicroSeconds ()/125;
	uint32_t rb_count = 1;
	FILE* log_file;
	char fname[255];
	sprintf(fname, "UE_%lu_UL_SINR_dB.txt", imsi);
	log_file = fopen(fname, "a");
	Values::iterator it = sinr.ValuesBegin();
	while(it!=sinr.ValuesEnd())
	{
		//fprintf(log_file, "%d\t%d\t%f\t \n", slot_count/2, rb_count, 10*log10(*it));
		fprintf(log_file, "%lu\t%lu\t%d\t%f\t \n",slot_count/8+1, slot_count%8+1, rb_count, 10*log10(*it));
		rb_count++;
		it++;
	}
	fflush(log_file);
	fclose(log_file);
	//phyStats->ReportInterferenceTrace (imsi, sinr);
	//phyStats->ReportPowerTrace (imsi, power);
}

void
MmWavePhyRxTrace::ReportInterferenceTrace (uint64_t imsi, SpectrumValue& sinr)
{
	uint64_t slot_count = Now().GetMicroSeconds ()/125;
	uint32_t rb_count = 1;
	FILE* log_file;
	char fname[255];
	sprintf(fname, "UE_%llu_SINR_dB.txt", (long long unsigned ) imsi);
	log_file = fopen(fname, "a");
	Values::iterator it = sinr.ValuesBegin();
	while(it!=sinr.ValuesEnd())
	{
		//fprintf(log_file, "%d\t%d\t%f\t \n", slot_count/2, rb_count, 10*log10(*it));
		fprintf(log_file, "%llu\t%llu\t%d\t%f\t \n",(long long unsigned) slot_count/8+1, (long long unsigned) slot_count%8+1, rb_count, 10*log10(*it));
		rb_count++;
		it++;
	}
	fflush(log_file);
	fclose(log_file);
}

void
MmWavePhyRxTrace::ReportPowerTrace (uint64_t imsi, SpectrumValue& power)
{

	uint32_t slot_count = Now().GetMicroSeconds ()/125;
	uint32_t rb_count = 1;
	FILE* log_file;
	char fname[255];
	printf (fname, "UE_%llu_ReceivedPower_dB.txt", (long long unsigned) imsi);
	log_file = fopen(fname, "a");
	Values::iterator it = power.ValuesBegin();
	while(it!=power.ValuesEnd())
	{
		fprintf(log_file, "%llu\t%llu\t%d\t%f\t \n",(long long unsigned) slot_count/8+1,(long long unsigned) slot_count%8+1, rb_count, 10*log10(*it));
		rb_count++;
		it++;
	}
	fflush(log_file);
	fclose(log_file);
}

void
MmWavePhyRxTrace::ReportPacketCountUeCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path,
			UePhyPacketCountParameter param)
{
	phyStats->ReportPacketCountUe (param);
}
void
MmWavePhyRxTrace::ReportPacketCountEnbCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path,
		EnbPhyPacketCountParameter param)
{
	phyStats->ReportPacketCountEnb (param);
}

void
MmWavePhyRxTrace::ReportDownLinkTBSize (Ptr<MmWavePhyRxTrace> phyStats, std::string path,
		uint64_t imsi, uint64_t tbSize)
{
	phyStats->ReportDLTbSize (imsi, tbSize);
}



void
MmWavePhyRxTrace::ReportPacketCountUe (UePhyPacketCountParameter param)
{
	FILE* log_file;
	char fname[255];
	sprintf (fname,"UE_%llu_Packet_Trace.txt", (long long unsigned) param.m_imsi);
	log_file = fopen (fname, "a");
	if (param.m_isTx)
	{
		fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, param.m_noBytes, 0);
	}
	else
	{
		fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, 0, param.m_noBytes);
	}

	fflush(log_file);
	fclose(log_file);

}

void
MmWavePhyRxTrace::ReportPacketCountEnb (EnbPhyPacketCountParameter param)
{
	FILE* log_file;
	char fname[255];
	sprintf (fname,"BS_%llu_Packet_Trace.txt",(long long unsigned) param.m_cellId);
	log_file = fopen (fname, "a");
	if (param.m_isTx)
	{
		fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, param.m_noBytes, 0);
	}
	else
	{
		fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, 0, param.m_noBytes);
	}

	fflush(log_file);
	fclose(log_file);
}

void
MmWavePhyRxTrace::ReportDLTbSize (uint64_t imsi, uint64_t tbSize)
{
	FILE* log_file;
	char fname[255];
	sprintf (fname,"UE_%llu_Tb_Size.txt", (long long unsigned) imsi);
	log_file = fopen (fname, "a");

	fprintf (log_file, "%lu \t %lu\n", Now().GetMicroSeconds (), tbSize);
	fprintf (log_file, "%lld \t %llu \n",(long long int) Now().GetMicroSeconds (), (long long unsigned) tbSize);
	fflush(log_file);
	fclose(log_file);
}

void
MmWavePhyRxTrace::RxPacketTraceUeCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path, RxPacketTraceParams params)
{
	if (!m_rxPacketTraceFile.is_open())
	{
		m_rxPacketTraceFilename = "RxPacketTrace.txt";
		m_rxPacketTraceFile.open(m_rxPacketTraceFilename.c_str ());
		if (!m_rxPacketTraceFile.is_open())
		{
			NS_FATAL_ERROR ("Could not open tracefile");
		}
	}
	m_rxPacketTraceFile << "DlRxTrace\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
			<< "\t" << (unsigned)params.m_numSym
			<< "\t" << params.m_rnti << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
			<< params.m_sinr << "\t" << params.m_tbler <<" \t" << params.m_corrupt << "\t" << params.m_sinrMin << std::endl;

	if (params.m_corrupt)
	{
		NS_LOG_DEBUG ("DL TB error\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
		    << "\t" << (unsigned)params.m_numSym
				<< "\t" << params.m_rnti << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
				<< params.m_sinr << "\t" << params.m_tbler << "\t" << params.m_corrupt);
	}
}
void
MmWavePhyRxTrace::RxPacketTraceEnbCallback (Ptr<MmWavePhyRxTrace> phyStats, std::string path, RxPacketTraceParams params)
{
	if (!m_rxPacketTraceFile.is_open())
	{
		m_rxPacketTraceFilename = "RxPacketTrace.txt";
		m_rxPacketTraceFile.open(m_rxPacketTraceFilename.c_str ());
		if (!m_rxPacketTraceFile.is_open())
		{
			NS_FATAL_ERROR ("Could not open tracefile");
		}
	}
	m_rxPacketTraceFile << "UlRxTrace\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
				<< "\t" << (unsigned)params.m_numSym
				<< "\t" << params.m_rnti << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
				<< params.m_sinr << "\t" << params.m_tbler <<" \t" << params.m_corrupt << "\t" << params.m_sinrMin << std::endl;

		if (params.m_corrupt)
		{
			NS_LOG_DEBUG ("UL TB error\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
			    << "\t" << (unsigned)params.m_numSym
					<< "\t" << params.m_rnti << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
					<< params.m_sinr << "\t" << params.m_tbler << "\t" << params.m_corrupt << "\t" << params.m_sinrMin);
		}
}

} /* namespace ns3 */
