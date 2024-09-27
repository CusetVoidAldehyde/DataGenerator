#ifndef _GENERATOR_H
#define _GENERATOR_H "generator.h v2.3 04/01/2024"
#define TIMEALL TESTPOINT_TIMELIMIT_MILLISECONDS + TESTPOINT_TIMELIMIT_EXTRAMILLISECONDS

#include <bits/stdc++.h>
#include <windows.h>
#include <tlhelp32.h>

std::default_random_engine dre (
	std::chrono::duration_cast<std::chrono::nanoseconds>
	(std::chrono::system_clock::now().time_since_epoch()).count()
);

extern const int TESTPOINT_BEGIN;
extern const int TESTPOINT_END;
extern const clock_t TESTPOINT_TIMELIMIT_MILLISECONDS;
extern const clock_t TESTPOINT_TIMELIMIT_EXTRAMILLISECONDS;
extern const int WAIT_FOR_THREAD_OPEN_MILLI;
extern const char *SOLUTION_EXE;
extern const std::vector<std::string> ALT_SOL_EXE;
extern const char *CHECKER_EXE;
extern const char *TESTPOINT_DIR;
extern const char *ALT_SOL_OUTPUT_DIR;
extern const bool DO_GENERATE;
extern const bool RUN_SOLUTION;
extern const bool RUN_ALT_SOL;
extern const bool DO_CHECK;

void execute_exe(std::string exe_name) {
	system(exe_name.c_str());
}

void kill_thread(std::string thread_name) {
	std::string cmd = std::string("taskkill /f /im ");
	cmd += thread_name;
	system(cmd.c_str());
}

DWORD GetProcessidFromName(LPCTSTR name) {
    PROCESSENTRY32 pe;
    DWORD id = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe))
        return 0;
    while(1) {
        pe.dwSize = sizeof(PROCESSENTRY32);
        if(Process32Next(hSnapshot, &pe) == FALSE)
            break;
        if(strcmp(pe.szExeFile, name) == 0) {
            id = pe.th32ProcessID;
            break;
        }
    }
    CloseHandle(hSnapshot);
    return id;
}

clock_t main_solve (
	std::string in_file,
	std::string out_file,
	std::string sol_cmd
) {
	freopen(in_file.c_str(), "rb", stdin);
	freopen(out_file.c_str(), "wb", stdout);
	
	std::thread tr = std::thread(execute_exe, sol_cmd);
	tr.detach();

	clock_t time_begin = clock();

	Sleep(WAIT_FOR_THREAD_OPEN_MILLI);

	int tall = TIMEALL;
	bool if_full = false;
	// fprintf(stderr, "DWORD: %d, exe: %s\n", status, sol_cmd.c_str());

	for (int i = 1, status = GetProcessidFromName(LPCSTR(sol_cmd.c_str())); 
		i <= tall / 15 && status; 
		i++, status = GetProcessidFromName(LPCSTR(sol_cmd.c_str()))
	) {
		if (status) Sleep(1);
		if (i == tall) if_full = true;
		// fprintf(stderr, "Run.\n");
	}

	clock_t time_used = clock() - time_begin - WAIT_FOR_THREAD_OPEN_MILLI;

	fclose(stdin);
	fclose(stdout);

	freopen("Debug.dll", "wb", stdout);
	Sleep(100);
	if (GetProcessidFromName(LPCSTR(sol_cmd.c_str())))
		kill_thread(sol_cmd);
	fclose(stdout);

	if (time_used <= TIMEALL && !if_full) return time_used;
	

	return TIMEALL;
}

void main_interact (
	void (*generator)(int)
) {
	if(RUN_SOLUTION) {
		system((std::string() + "IF NOT EXIST " + TESTPOINT_DIR + " MKDIR " + TESTPOINT_DIR).c_str());
	}
	
	if(ALT_SOL_EXE.size()) {
		system((std::string() + "IF NOT EXIST " + ALT_SOL_OUTPUT_DIR + " MKDIR " + ALT_SOL_OUTPUT_DIR).c_str());
	}
	
	for(int tp = TESTPOINT_BEGIN; tp <= TESTPOINT_END; ++tp) {
		std::string in_file = std::string() + TESTPOINT_DIR + "\\" + std::to_string(tp) + ".in";
		std::string out_file = std::string() + TESTPOINT_DIR + "\\" + std::to_string(tp) + ".out";
		
		if(DO_GENERATE && generator) {
			freopen(in_file.c_str(), "wb", stdout);
			generator(tp);
			fclose(stdout);
		} // DO_GENERATE
		
		if(RUN_SOLUTION && SOLUTION_EXE) {
			clock_t time_used = main_solve(in_file, out_file, SOLUTION_EXE);
			fprintf(stderr, "testpoint %d: sol, %ld ms\n", tp, time_used);
		} // SOLUTION_EXE
		
		if(RUN_ALT_SOL) {
			for(int idx = 0; idx < ALT_SOL_EXE.size(); idx++) {
				std::string out_file_another = std::string() + ALT_SOL_OUTPUT_DIR + "\\" + std::to_string(tp) + "_" + std::to_string(idx) + ".out";
				
				clock_t time_used = main_solve(in_file, out_file_another, ALT_SOL_EXE[idx]);
				fprintf(stderr, "testpoint %d: alt_sol %d, %ld ms\n", tp, idx + 1, time_used);
				
				if((time_used != TIMEALL) && DO_CHECK && CHECKER_EXE) {
					Sleep(100);
					system((std::string() + CHECKER_EXE + " " + in_file + " " + out_file_another + " " + out_file).c_str());
				} else if (DO_CHECK && CHECKER_EXE) {
					fprintf(stderr, "alt_sol %d \033[1;34mTime Limit Exceeded.\033[0m\n", idx + 1);
				} // DO_CHECK
			} // ALT_SOL_EXE vector
		} // RUN_ALT_SOL
		
		fprintf(stderr, "\n");
	} // main loop
}

template<typename Type,
	typename = typename std::enable_if<std::is_integral<Type>::value>::type>
Type rand_integer(const Type &min, const Type &max) {
	std::uniform_int_distribution<Type> rndev(min, max);
	return rndev(dre);
}

template<typename Type,
	typename = typename std::enable_if<std::is_floating_point<Type>::value>::type>
Type rand_real(const Type &min, const Type &max) {
	std::uniform_real_distribution<Type> rndev(min, max);
	return rndev(dre);
}

int rand(int l, int r) { return rand_integer(l, r); }
long long rand(long long l, long long r) { return rand_integer(l, r); }
double rand(double l, double r) { return rand_real(l, r); }

#endif
