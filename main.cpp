#include "stdafx.h"
#include "ITask.h"

#include <chrono> 
#include <conio.h>
#include <filesystem>
#include <fstream>
#include <vector>


size_t DefABC[3][3] = { { 3, 7 },{ 1, 3 },{ 1, 10 }};		

int main()
{
	std::vector< std::pair<std::shared_ptr<ThreadKoeff>, std::shared_ptr<std::thread> > > CoefThreads;
	
	//Creating coef. objects, starting their threads
	for (size_t i = 0; i < 3; ++i) {
		auto pTK = std::make_shared<ThreadKoeff>(DefABC[i][0], DefABC[i][1]);
		CoefThreads.emplace_back(std::make_pair(pTK, std::make_shared<std::thread>(&ThreadKoeff::ThreadStart, pTK)));
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::ofstream outfile;
	try {
		outfile.open("out.txt");
	}
	catch (std::tr2::sys::filesystem_error& e) {
		std::cout << e.what() << '\n';
		return 1;
	}

	// Calculating loop
	unsigned long t = 0;
	while (true) {
		for (double i = 0.; i < 6.24; i += 0.2) {
			for (size_t i = 0; i < 3; ++i) CoefThreads[i].first->Click(i);

			std::this_thread::sleep_for(std::chrono::seconds(1));

			double y = CoefThreads[0].first->GetKoef() * CoefThreads[1].first->GetKoef() * sin(CoefThreads[2].first->GetKoef() * ++t);

			std::cout << "[" << t << ", " << y << "]" << std::endl;
			outfile << "[" << t << ", " << y << "]" << std::endl;
		}

		// User interface
		std::cout << "g - Go on, e - Enter new data, q - stop: ";
		char c;
		std::cin >> c;
		if (c == 'e') {
			std::cout << "Enter new A, B, C:";
			double X; 
			for (size_t j = 0; j < 3; ++j) {
				std::cin >> X;
				CoefThreads[j].first->ChangeKoef(X);
			}
		}
		else if (c == 'q') {
			break;
		}
	}

	outfile.close();

	// Stopping coef. threads
	for (size_t i = 0; i < 3; ++i) {
		CoefThreads[i].first->Stop();
		if (CoefThreads[i].second && CoefThreads[i].second->joinable())
			CoefThreads[i].second->join();
	}
	std::cout << "End";
	return 0;
}
