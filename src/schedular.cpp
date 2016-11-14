/**
 * Header files
 */
#include <fstream>

#include "support.h"
#include "log.h"
#include "schedular.h"

/**
 * Constructor
 */
CJob::CJob(unsigned int nType, unsigned int nJob, unsigned int nArrival, unsigned int nBurst)
{
	m_nType = nType;
	m_nJob = nJob;
	m_nArrival = nArrival;
	m_nBurst = nBurst;
	m_nTime = 0;
	m_nRunning = 0;
}

/**
 * Copy constructor
 */
CJob::CJob(const CJob& cJob)
{
	SetType(cJob.GetType());
	SetJob(cJob.GetJob());
	SetBurst(cJob.GetBurst());
	SetArrival(cJob.GetArrival());
	SetTime(cJob.GetTime());
	SetRunning(cJob.GetRunning());
}

/**
 * Comparitors for priority_queue
 */
bool operator > (const CJob& lhs, const CJob& rhs)
{
	bool bRes = false;
	if ((lhs.m_nType & FIFO) && (rhs.m_nType & FIFO))		/* In case of FCFS, sort according to arrival time */
		bRes = lhs.m_nArrival < rhs.m_nArrival;
	else if ((lhs.m_nType & SRJF) && (rhs.m_nType & SRJF))	/* In case of SRJF, sort according to burst time */
		bRes = lhs.m_nBurst < rhs.m_nBurst;
	return bRes;
}

/**
 * Comparitors for priority_queue
 */
bool operator < (const CJob& lhs, const CJob& rhs)
{
	bool bRes = false;
	if ((lhs.m_nType & FIFO) && (rhs.m_nType & FIFO))		/* In case of FCFS, sort according for arrival time */
		bRes = lhs.m_nArrival > rhs.m_nArrival;
	else if ((lhs.m_nType & SRJF) && (rhs.m_nType & SRJF))	/* In case of SRJF, sort according to burst time */
		bRes = lhs.m_nBurst > rhs.m_nBurst;
	return bRes;
}

/**
 * Constructor
 */
CSchedular::CSchedular(unsigned int nType, unsigned int nTimeQuantum, char* pFileName, unsigned int nJobs, bool bVerbose)
{
	m_nType = nType;
	m_nTimeQuantum = nTimeQuantum;
	m_pFileName = pFileName;
	m_bVerbose = bVerbose;
	m_nTime = 0;
	m_nJobs = nJobs;
}

/**
 * Start:
 * Main entry point in the class
 */
int CSchedular::Start()
{
	debug_log("Entering %s ...", __FUNCTION__);
	int nRes = 0;
	try {
		if (IsRandom())		/* If we have to create random jobs */
			Random();	/* Create random jobs */
		else			/* Or read jobs from file */
			ReadFile();	/* Read the jobs from file */
#if DEBUG
		DisplayJobs();		/* Display the jobs */
#endif // DEBUG
		Execute();		/* Execute the algorithm */
		Clear();		/* Clear the data structures */
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);
	return nRes;
}

/**
 * ReadFile:
 *
 * Read jobs from file
 */
int CSchedular::ReadFile()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	int nRes = 0;
	if (m_pFileName == NULL) {
		err_printf("Invalid parameter, filename not found");
		return -1;
	}
	std::ifstream inputFile(m_pFileName);	/* open the file for reading */
	std::string csLine;
	std::string csItem;
	unsigned int nJob = 0;
	unsigned int nArrival = 0;
	unsigned int nBurst = 0;
	try {
		while (std::getline(inputFile, csLine)) {	/* read one line from the file */
			debug_log(csLine.c_str());		/* debugging log to display the line */
			int nIndex = csLine.find(",");		/* find , as separator */
			csItem = csLine.substr(0, nIndex);	/* separate the job numbfer */
			debug_log("Job # %s", csItem.c_str());	/* debugging log to display the job number */
			nJob = atoi(csItem.c_str());		/* convert to integer */
			nIndex ++;
			csItem = csLine.substr(nIndex, csLine.rfind(",") - nIndex);	/* get remaining line */
			debug_log("Arrival time = %s", csItem.c_str());			/* debugging log for arrival time */
			nArrival = atoi(csItem.c_str());				/* convert to integer */
			nIndex = csLine.rfind(",");
			nIndex ++;
			csItem = csLine.substr(nIndex, csLine.length());	/* Find the burst time */
			debug_log("Burst time = %s", csItem.c_str());		/* debugging log for burst time */
			nBurst = atoi(csItem.c_str());				/* conver to integer */
			
			debug_log("Inserting %d", nJob);		/* debugging log for job number */
			CJob cJob(m_nType, nJob, nArrival, nBurst);	/* create object of the job */
			m_cList.push_back(cJob);			/* push the object to list */
			debug_log("List size now: %d", m_cList.size());	/* display the list current size */
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * DisplayJobs:
 *
 * Display the available jobs to process
 */
int CSchedular::DisplayJobs()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	int nRes = 0;
	try {
		for (std::vector<CJob>::const_iterator cIter = m_cList.begin();	/* Iterate over the list */
			cIter != m_cList.end();
			++ cIter) {
			log_message("Job: %d, Arrival: %d, Time: %d",		/* Display job, arrival time and burst time */
				    (*cIter).GetJob(),
				    (*cIter).GetArrival(),
				    (*cIter).GetBurst());
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * Clear:
 * 
 * Clear the list
 */
void CSchedular::Clear()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	try {
		m_cList.clear();			/* clear the list */
		SetTime(0);				/* reset the total time */
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s...", __FUNCTION__);	/* trace log */
}

/**
 * Execute:
 *
 * Execute the algorithm
 */
int CSchedular::Execute()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	int nRes = 0;
	try {
		if (IsFIFO()) {				/* Is FCFS? */
			nRes = ExecuteFCFS();		/* Execute FCFS algorithm */
		}
		else if (IsSRJF()) {			/* Is SRJF? */
			nRes = ExecuteSRJF();		/* Execute SRJF algorithm */
		}
		else if (IsRoundRobin()) {		/* Is RoundRobin? */
			nRes = ExecuteRR();		/* Execute the round robin algorithm */
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * ExecuteFCFS:
 *
 * Execute the FCFS algorithm
 */
int CSchedular::ExecuteFCFS()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	int nRes = 0;
	try {
		log_message("sched -F for %s", m_pFileName ? m_pFileName : "random jobs");	/* print the command to console */

		int nTime = 0;
		for (std::vector<CJob>::iterator Iter = m_cList.begin();	/* For each job */
			Iter != m_cList.end();
			++ Iter) {
			int nArrival = (*Iter).GetArrival();		/* Get the arrival time */
			if (nArrival <= nTime)				/* Just to set the proper time */
				nArrival = nTime;
			if (!m_bVerbose)				/* If not verbose mode, display the information */
				log_message("%d %d", (*Iter).GetJob(), nArrival + (*Iter).GetBurst());
			(*Iter).SetRunning(nTime == 0 ? nArrival : nTime); /* Set the running time */
			nTime = nArrival + (*Iter).GetBurst();		/* Get the total time */
			(*Iter).SetTime(nTime);				/* update the total time for this job */
		}
		SetTime(nTime);		/* update the total time for all jobs */

		if (m_bVerbose) {
			for (int nTime = 0; nTime <= GetTime(); ++ nTime) {	/* loop for time, unless all jobs are completed */
				int nJob = -1;
				for (std::vector<CJob>::const_iterator cIter = m_cList.begin();	/* for each job */
					cIter != m_cList.end();
					++ cIter) {

					if (nTime == (*cIter).GetArrival()) {
						/**
						 * A new job has arrived.
						 * Insert this job in the Q
						 */
						m_cQueue.push((*cIter));
						log_message("At time %d, job %d READY", nTime, (*cIter).GetJob());
					}
					if (!m_cQueue.empty() &&
						nTime == m_cQueue.top().GetRunning() &&
						nJob != m_cQueue.top().GetJob()) {

						/**
						 * If Q isn't empty AND
						 * current time is the time for this job to start RUNNING AND
						 * This message hasn't been displayed for this job
						 * Display the verbose log message
						 */
						nJob = m_cQueue.top().GetJob(); /* we displayed the message for this job */
						log_message("At time %d, job %d READY->RUNNING", nTime, m_cQueue.top().GetJob());
					}
					if (nTime == (*cIter).GetTime()) {
						/**
						 * The time has been reached for this job.
						 * Remove this job from the Q
						 * Log the message
						 */
						m_cQueue.pop();
						log_message("At time %d, job %d RUNNING->TERMINATED", nTime, (*cIter).GetJob());
					}
				}
			}
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * ExecuteSRJF:
 *
 * Execute the SRJF algorithm
 */
int CSchedular::ExecuteSRJF()
{
	debug_log("Entering %s ...", __FUNCTION__);		/* trace log */
	int nRes = 0;
	try {
		log_message("sched -S for %s", m_pFileName ? m_pFileName : "random jobs");	/* print the command to console */

		int nTime = 0;
		for (std::vector<CJob>::iterator Iter = m_cList.begin();	/* for each job */
			Iter != m_cList.end();
			++ Iter) {
			int nArrival = (*Iter).GetArrival();	/* Get arrival time */
			nTime = nArrival + (*Iter).GetBurst();	/* Update the total time */
			(*Iter).SetTime(nTime);			/* Set the total time */
			if (Iter == m_cList.begin())		/* Just for first job */
				SetTime((*Iter).GetArrival());	/* Set the total time for all jobs, to be updated later */
		}

		nTime = GetTime();
		for (; nTime <= GetTime(); ++ nTime) {		/* loop for time, till all jobs are completed */
			int nJob = -1;
			for (std::vector<CJob>::iterator Iter = m_cList.begin(); /* for each job */
				Iter != m_cList.end();
				++ Iter) {
				if ((*Iter).GetBurst() == 0)	/* If this job's burst time is done, just leave it */
					continue;
				if (nTime == (*Iter).GetArrival()) {
					/**
					 * A job is arrived, put it in Queue
					 */
					if (m_bVerbose)		/* If it's verbose mode, display the message */
						log_message("At time %d, job %d READY", nTime, (*Iter).GetJob());
					if (!m_cQueue.empty()) {
						/**
						 * If Q isn't empty AND
						 * Current job's burst time is smallar then the running job
						 * preempt the job, and log the message
						 */
						if ((*Iter).GetBurst() < m_cQueue.top().GetBurst()) {
							if (m_bVerbose)
								log_message("At time %d, job %d RUNNING->READY", nTime, m_cQueue.top().GetJob());
						}
					}
					m_cQueue.push((*Iter));	/* push the job in Q */
				}
				if (!m_cQueue.empty() &&
					(*Iter).GetJob() == m_cQueue.top().GetJob()) {

					/**
					 * If Q isn't empty and we are processing the same job
					 */
					if (nJob != m_cQueue.top().GetJob() &&
						nTime == m_cQueue.top().GetArrival()) {

						/**
						 * If this job has not been logged as running
						 * just log the message in verbose mode
						 * update the total running time for all jobs in order to update the main loop
						 */
						nJob = m_cQueue.top().GetJob();
						if (m_bVerbose)
							log_message("At time %d, job %d READY->RUNNING", nTime, m_cQueue.top().GetJob());
						SetTime(GetTime() + m_cQueue.top().GetBurst());
					}
					else {
						/**
						 * We need to update the running time
						 * we have to update the burst time
						 * because the Queue is sorted based on burst time
						 */
						CJob cJob(m_cQueue.top());
						m_cQueue.pop();
						cJob.SetBurst(cJob.GetBurst() - 1);
						cJob.SetRunning(cJob.GetRunning() + 1);
						m_cQueue.push(cJob);
					}
				}
				if (!m_cQueue.empty() &&
					(*Iter).GetJob() == m_cQueue.top().GetJob() &&
					m_cQueue.top().GetBurst() == 0) {

					/**
					 * If Q isn't empty
					 * We are processing the same job
					 * The job burst time has been finished
					 * Log the termination message in case of verbose
					 */
					if (m_bVerbose)
						log_message("At time %d, job %d RUNNING->TERMINATED", nTime, m_cQueue.top().GetJob());
					(*Iter).SetTime(nTime);
					(*Iter).SetBurst(m_cQueue.top().GetBurst());
					m_cQueue.pop();		/* remove the item from Q */
					if (!m_cQueue.empty()) {

						/**
						 * We still have more items in Q
						 * Get the top item from Q
						 * And put it back on Running
						 */
						nJob = m_cQueue.top().GetJob();
						if (m_bVerbose)
							log_message("At time %d, job %d READY->RUNNING", nTime, m_cQueue.top().GetJob());
						SetTime(GetTime() - m_cQueue.top().GetRunning() + m_cQueue.top().GetBurst());
						/**
						 * Terminate the loop
						 * Otherwise it will iterate for this job one
						 * and increment it's time, even though we are at same time
						 * So this job time should increment at next iteration
						 */
						break;
					}
				}
			}
		}
log_message("Time : %d", GetTime());
		if (!m_bVerbose) {

			/**
			 * Not a verbose mode
			 * Just display the normal information
			 * calculated from above algorithm
			 */
			debug_log("List size: %d", m_cList.size());
			for (std::vector<CJob>::const_iterator cIter = m_cList.begin();
				cIter != m_cList.end();
				++ cIter) {
				log_message("%d %d", (*cIter).GetJob(), (*cIter).GetTime());
			}
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * ExecuteRR:
 * 
 * Execute the round robin algorithm
 */
int CSchedular::ExecuteRR()
{
	debug_log("Entering %s ...", __FUNCTION__);	/* trace log */
	int nRes = 0;
	try {
		log_message("sched -R %d for %s", GetTimeQuantum(), m_pFileName ? m_pFileName : "random jobs");	/* print command */

		int nTime = 0;
		for (std::vector<CJob>::iterator Iter = m_cList.begin();	/* for each job */
			Iter != m_cList.end();
			++ Iter) {
			int nArrival = (*Iter).GetArrival();			/* Get the arrival time */
			nTime = nArrival + (*Iter).GetBurst();			/* update the total time */
			(*Iter).SetTime(nTime);					/* Set the total time */
			if (Iter == m_cList.begin())				/* just for the first job */
				SetTime((*Iter).GetArrival());			/* Set total time for all jobs, we will udpate it later */
		}

		nTime = GetTime();
		int nJob = -1;
		for (; nTime <= GetTime(); ++ nTime) {				/* For the time till all jobs are finished */
			for (std::vector<CJob>::iterator Iter = m_cList.begin();/* For all jobs */
				Iter != m_cList.end();
				++ Iter) {
				if ((*Iter).GetBurst() == 0)			/* If job's burst is done, just skipp it */
					continue;
				if (!m_cRRQueue.empty() &&
					m_cRRQueue.size() > 1 &&
					(*Iter).GetJob() == m_cRRQueue.front().GetJob() &&
					m_cRRQueue.front().GetRunning() != 0 &&
					m_cRRQueue.front().GetBurst() - 1 != 0 &&
					(m_cRRQueue.front().GetRunning() + 1) % GetTimeQuantum() == 0) {

					/**
					 * If Q isn't empty AND
					 * We are working on same job AND
					 * Job's running time has started AND
					 * Job's burst time isn't complete yet AND
					 * Time quantum has been finished for this job
					 * 
					 * Put it to back of Q, and display the verbose message
					 */ 
					if (m_bVerbose)
						log_message("At time %d, job %d RUNNING->READY", nTime, m_cRRQueue.front().GetJob());
					CJob cJob(m_cRRQueue.front());		/* get the top job from Q */
					m_cRRQueue.pop();			/* pop the job from Q */
					cJob.SetBurst(cJob.GetBurst() - 1);	/* Update the burst time */
					cJob.SetRunning(cJob.GetRunning() + 1);	/* Update the running time */
					m_cRRQueue.push(cJob);			/* Push the job at end of Q */
					Iter = m_cList.begin();	/* Get back to start of jobs (we may miss earlier job for current time */
				}
				if (nTime == (*Iter).GetArrival()) {
					/**
					 * A job is arrived, put it in Queue
					 */
					if (m_bVerbose)
						log_message("At time %d, job %d READY", nTime, (*Iter).GetJob());
					m_cRRQueue.push(*Iter);
				}
				if (!m_cRRQueue.empty() &&
					(*Iter).GetJob() == m_cRRQueue.front().GetJob()) {

					/**
					 * Q isn't empty
					 * This job is in the running state
					 */
					if (nJob != m_cRRQueue.front().GetJob()) {

						/**
						 * We haven't displayed the ready->running message for this job
						 * so display the verbose message
						 * Set the total time for all jobs to keep the main loop running
						 */
						nJob = m_cRRQueue.front().GetJob();
						if (m_bVerbose)
							log_message("At time %d, job %d READY->RUNNING", nTime, nJob);
						SetTime(GetTime() + (*Iter).GetBurst());
					}
					else {
						/**
						 * We need to update the running time
						 * we have to update the burst time
						 * because the Queue is sorted based on burst time
						 */
						m_cRRQueue.front().SetBurst(m_cRRQueue.front().GetBurst() - 1);
						m_cRRQueue.front().SetRunning(m_cRRQueue.front().GetRunning() + 1);
					}
				}
				if ((*Iter).GetJob() == m_cRRQueue.front().GetJob() &&
					m_cRRQueue.front().GetBurst() == 0) {

					/**
					 * If current job's burst time is finished
					 * Log message to terminate the job
					 */
					if (m_bVerbose)
						log_message("At time %d, job %d RUNNING->TERMINATED", nTime, (*Iter).GetJob());

					/**
					 * Update the time for this job
					 * Set the running and burst time accordingly
					 */
					(*Iter).SetTime(nTime);
					(*Iter).SetRunning(m_cRRQueue.front().GetRunning());
					(*Iter).SetBurst(0);

					SetTime(GetTime() - (*Iter).GetRunning());	/* set the time for main loop */
					m_cRRQueue.pop();				/* remove this job from Q */
					if (!m_cRRQueue.empty()) {

						/**
						 * If Q isn't empty
						 * Put the first job in Q back to running
						 */
						nJob = m_cRRQueue.front().GetJob();
						if (m_bVerbose)
							log_message("At time %d, job %d READY->RUNNING", nTime, nJob);
						SetTime(GetTime() + m_cRRQueue.front().GetBurst());
						/**
						 * Terminate the loop
						 * Otherwise it will iterate for this job one
						 * and increment it's time, even though we are at same time
						 * So this job time should increment at next iteration
						 */
						break;
					}

				}
			}
		}

		if (!m_bVerbose) {

			/**
			 * Not a verbose mode
			 * Just display the normal information
			 * calculated from above algorithm
			 */
			debug_log("List size: %d", m_cList.size());
			for (std::vector<CJob>::const_iterator cIter = m_cList.begin();
				cIter != m_cList.end();
				++ cIter) {
				log_message("%d %d", (*cIter).GetJob(), (*cIter).GetTime());
			}
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* trace log */
	return nRes;
}

/**
 * Random:
 *
 * Create random jobs
 * with their arrival time, burst time
 * and populate them to the list
 */
int CSchedular::Random()
{
	debug_log("Entering %s ...", __FUNCTION__);
	int nRes = 0;
	try {
		unsigned int nArrival = 0;
		unsigned int nPreArrival = 0;
		unsigned int nBurst = 0;
		for (size_t nJob = 0; nJob < m_nJobs; ++ nJob) {	/* creating jobs */
			srand(time(NULL));			/* create random data */
			nArrival = rand() % (6 - 1) + 1;	/* create random arrival time */
			nArrival += nPreArrival;		/* increment it to previous arrival */
			srand(time(NULL));
			nBurst = rand() % (30 - 1) + 1;		/* create random burst time */

			log_message("creating job: %d, Arrival %d, Burst: %d",
				  nJob,
				  nArrival,
				  nBurst);
			CJob cJob(m_nType, nJob, nArrival, nBurst);
			m_cList.push_back(cJob);
			nPreArrival = nArrival;
			sleep(1);
		}
	}
	catch (std::exception e) {
		perr_printf(e.what());
	}
	catch (...) {
		err_printf("Unknown Exception...");
	}
	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);
	return nRes;
}
