if not exist vcpkg\ (
	git clone https://github.com/microsoft/vcpkg
	call .\vcpkg\bootstrap-vcpkg.bat
)

.\vcpkg\vcpkg install boost-iostreams:x64-windows
.\vcpkg\vcpkg install boost-any:x64-windows
.\vcpkg\vcpkg install boost-algorithm:x64-windows
.\vcpkg\vcpkg install boost-uuid:x64-windows
.\vcpkg\vcpkg install boost-interprocess:x64-windows
.\vcpkg\vcpkg install zlib:x64-windows
.\vcpkg\vcpkg install blosc:x64-windows
.\vcpkg\vcpkg install tbb:x64-windows
