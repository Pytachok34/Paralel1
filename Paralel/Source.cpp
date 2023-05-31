#include <iostream> 
#include <Windows.h> 
#include <process.h> 
#include <iomanip> 
#include <vector> 
#include <iomanip> 

const size_t NTHREAD = 3;
const size_t n = 9;

struct INFORM
{
	std::vector<int>* a;
	size_t left, right;
	bool check = true;
};

bool non_parallel_check(std::vector<int>& a)
{
	bool flag = true;
	size_t i(0);
	size_t size = a.size() - 1;
	while (i < size - 1 && flag)
	{
		if ((a[i] < 0 && a[i + 1] < 0) || (a[i] >= 0 && a[i + 1] >= 0))
			flag = false;
		++i;
	}
	return flag;
}

unsigned __stdcall check(void* arg)
{
	INFORM* inform = (INFORM*)arg;
	inform->check = true;
	size_t i = inform->left;
	size_t size = inform->right;
	while (i < size - 1 && inform->check == true)
	{
		if ((*(inform->a))[i] < 0 && (*(inform->a))[i + 1] < 0 || (*(inform->a))[i] >= 0 && (*(inform->a))[i + 1] >= 0)
			inform->check = false;
		++i;
	}
	_endthreadex(0);
	return 0;
}

bool parallel_check(std::vector<int>& a)
{
	HANDLE t[NTHREAD];
	unsigned ThreadID[NTHREAD];
	INFORM informs[NTHREAD];
	size_t N = n / NTHREAD;
	for (size_t i = 0; i < NTHREAD; ++i)
	{
		informs[i].a = &a;
		informs[i].left = i * N;
		informs[i].check = true;
		if (i == NTHREAD - 1)
			informs[i].right = n;
		else
			informs[i].right = (i + 1) * N + 1;
		t[i] = (HANDLE)_beginthreadex(nullptr, 0, &check, &informs[i], 0, nullptr);
	}
	bool global_flag = true;
	size_t i = (NTHREAD - 1) * N;
	size_t size = n - 1;
	while (i < size && global_flag)
	{
		if (a[i] < 0 && a[i + 1] < 0 || a[i] >= 0 && a[i + 1] >= 0)
			global_flag = false;
		++i;
	}
	WaitForMultipleObjects(NTHREAD, t, true, INFINITE);
	for (size_t i = 0; i < NTHREAD; ++i)
	{
		if (!informs[i].check)
			global_flag = false;
		CloseHandle(t[i]);
	}
	return global_flag;
}

int main()
{
	std::vector<int> vec = { -2,3,1,5,-2,7,-9,3,-1 };
	if (non_parallel_check(vec))
		std::cout << "Yes \n";
	else
		std::cout << "No \n";
	if (parallel_check(vec))
		std::cout << "Yes \n";
	else
		std::cout << "No \n";
	std::cin.get();
	return 0;
}
