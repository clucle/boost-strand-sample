#include <iostream>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

void TestSingleThread()
{
	std::cout << "============== TestSingleThread ==============" << "\n";
	std::cout << "Main Thread Id :" << std::this_thread::get_id() << "\n";

	boost::asio::io_context io_context;
	boost::asio::post( io_context, []()
		{
			std::cout << "Post 1 Thread Id : " << std::this_thread::get_id() << "\n";
		} );

	boost::asio::post( io_context, []()
		{
			std::cout << "Post 2 Thread Id : " << std::this_thread::get_id() << "\n";
		} );

	boost::asio::post( io_context, []()
		{
			std::cout << "Post 3 Thread Id : " << std::this_thread::get_id() << "\n";
		} );

	io_context.run();
	std::cout << "==============================================" << "\n";
}


void TestMultiThreads()
{
	std::cout << "============== TestMultiThreads ==============" << "\n";
	std::cout << "Main Thread Id :" << std::this_thread::get_id() << "\n";

	boost::asio::io_context io_context;

	auto work = make_work_guard( io_context );

	int thread_cnt = 4;
	std::vector< std::thread > threads;
	for ( int i = 0; i < thread_cnt; ++i )
	{
		threads.emplace_back( [&]()
			{
				io_context.run();
			} );
	}

	std::mutex mutex;
	int user_cnt = 7;
	for ( int i = 0; i < user_cnt; ++i )
	{
		boost::asio::post( io_context, [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 1 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 1 Thread Id : " << std::this_thread::get_id() << " end\n";
			} );

		boost::asio::post( io_context, [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 2 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 2 Thread Id : " << std::this_thread::get_id() << " end\n";
			} );

		boost::asio::post( io_context, [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 3 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 3 Thread Id : " << std::this_thread::get_id() << " end\n";
			} );
	}

	work.reset();
	for ( auto& thread : threads )
	{
		thread.join();
	}

	std::cout << "==============================================" << "\n";
}


void TestStrand()
{
	std::cout << "================= TestStrand =================" << "\n";
	std::cout << "Main Thread Id :" << std::this_thread::get_id() << "\n";

	boost::asio::io_context io_context;
	auto work = make_work_guard( io_context );

	int thread_cnt = 4;
	std::vector< std::thread > threads;
	for ( int i = 0; i < thread_cnt; ++i )
	{
		threads.emplace_back( [&]()
			{
				io_context.run();
			} );
	}

	std::mutex mutex;
	int user_cnt = 7;
	std::vector< boost::asio::io_context::strand > strands;
	for ( int i = 0; i < user_cnt; ++i )
	{
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 1 ) );
		boost::asio::io_context::strand strand( io_context );
		strands.push_back( std::move( strand ) );
	}

	for ( int i = 0; i < user_cnt; ++i )
	{
		io_context.post( strands[ i ].wrap( [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 1 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 1 Thread Id : " << std::this_thread::get_id() << " end\n";
			} ) );

		io_context.post( strands[ i ].wrap( [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 2 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 2 Thread Id : " << std::this_thread::get_id() << " end\n";
			} ) );

		io_context.post( strands[ i ].wrap( [i, &mutex]()
			{
				std::cout << "User : " << i << " Post 3 Thread Id : " << std::this_thread::get_id() << " start\n";
				boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
				std::cout << "User : " << i << " Post 3 Thread Id : " << std::this_thread::get_id() << " end\n";
			} ) );
	}

	work.reset();
	for ( auto& thread : threads )
	{
		thread.join();
	}

	std::cout << "==============================================" << "\n";
}

int main()
{
	TestSingleThread();
	TestMultiThreads();
	TestStrand();

	return 0;
}