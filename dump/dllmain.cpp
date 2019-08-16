#include "memory.hpp"
#include <thread>
#include <chrono>

void __stdcall main_thread( HMODULE current_module )
{
	AllocConsole( );
	freopen_s( reinterpret_cast< FILE * * >( stdin ), "CONIN$", "r", stdin );
	freopen_s( reinterpret_cast< FILE * * >( stdout ), "CONOUT$", "w", stdout );
	SetConsoleTitleA( "[~] starcaller dumper" );

	if ( !memory::initialize( ) )
	{
		std::printf( "[-] fail, couldn't get image size/base\n" );
		std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
		FreeLibraryAndExitThread( current_module, EXIT_FAILURE );
	}

	std::printf( "[+] image start: 0x%llx\n", memory::module_info.first );
	std::printf( "[+] image size: 0x%llx\n", memory::module_info.second );

	const auto push_address = [ & ]( const char* name, const std::uintptr_t current_address, const std::int32_t relative = 4 )
	{
		if ( current_address <= 0x5 )
		{
			std::printf( "[!] couldn't get %s\n", name );
			return;
		}
		const auto relative_address = current_address + *reinterpret_cast< std::int32_t* >( current_address ) + relative;
		std::printf( "[+] %s found at: 0x%llx\n", name, relative_address - memory::module_info.first );
	};

	push_address( "game manager", memory::pattern_scan( "\x48\x8b\x05\x00\x00\x00\x00\x8b\x8e", "xxx????xx" ) + 0x3 );
	push_address( "status manager", memory::pattern_scan( "\x48\x8b\x2d\x00\x00\x00\x00\x48\x8b\x56\x18", "xxx????xxxx" ) + 0x3 );
	push_address( "environment area manager", memory::pattern_scan( "\x48\x8b\x1d\x00\x00\x00\x00\x48\x8b\xea\x48\x8b\xd1", "xxx????xxxxxx" ) + 0x3 );
	push_address( "render manager", memory::pattern_scan( "\x48\x89\x05\x00\x00\x00\x00\x48\x89\x18", "xxx????xxx" ) + 0x3 );
	push_address( "playermarkercomponent", memory::pattern_scan( "\x4c\x8d\x0d\x00\x00\x00\x00\x48\x00\x00\x00\x48\x8d\x8b\x00\x00\x00\x00\x4c\x00\x00\x48\x8d\x00\x00\x00\x00\x00\xe8\x00\x00\x00\x00", "xxx????x???xxx????x??xx?????x????" ) + 0x3 );
	push_address( "fov manager", memory::pattern_scan( "\x48\x8b\x05\x00\x00\x00\x00\x00\x00\x7c\x24\x70\x4c\x8b", "xxx??????xxxxx" ) + 0x3 );
	push_address( "profile manager", memory::pattern_scan( "\x48\x8b\x05\x00\x00\x00\x00\x33\xd2\x4c\x8b\x40\x78", "xxx????xxxxxx" ) + 0x3 );
	push_address( "network manager", memory::pattern_scan( "\x48\x8b\x3d\x00\x00\x00\x00\x48\x85\xff\x0f\x84\x00\x00\x00\x00\x48\x8b\xbf\x00\x00\x00\x00", "xxx????xxxxx????xxx????" ) + 0x3 );
	push_address( "trigger manager", memory::pattern_scan( "\x48\x8B\x3D\x00\x00\x00\x00\x48\x89\x46\x60", "xxx????xxxx" ) + 0x3 );

	while ( !GetAsyncKeyState( VK_END ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

	FreeConsole( );
	FreeLibraryAndExitThread( current_module, EXIT_SUCCESS );
}

bool __stdcall DllMain(HMODULE module_entry, std::uint32_t call_reason, void*) {
	if (call_reason == DLL_PROCESS_ATTACH)
		return CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main_thread), module_entry, 0, nullptr) != INVALID_HANDLE_VALUE;

	return false;
}
