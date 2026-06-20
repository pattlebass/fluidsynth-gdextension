# Build commands for FluidSynth

In `thirdparty/fluidsynth/`, run:

## Windows

```powershell
cmake -B build/windows-x86_64 -G "MinGW Makefiles" `
  -DCMAKE_INSTALL_PREFIX="./install/windows-x86_64" `
  -Dosal=cpp11 `
  -DCMAKE_BUILD_TYPE=Release `
  -Denable-alsa=OFF `
  -Denable-aufile=OFF `
  -DBUILD_SHARED_LIBS=ON `
  -Denable-dbus=OFF `
  -Denable-ipv6=OFF `
  -Denable-jack=OFF `
  -Denable-ladspa=OFF `
  -Denable-libinstpatch=OFF `
  -Denable-libsndfile=OFF `
  -Denable-midishare=OFF `
  -Denable-opensles=OFF `
  -Denable-oboe=OFF `
  -Denable-network=OFF `
  -Denable-oss=OFF `
  -Denable-dsound=OFF `
  -Denable-wasapi=OFF `
  -Denable-waveout=OFF `
  -Denable-winmidi=OFF `
  -Denable-sdl3=OFF `
  -Denable-pulseaudio=OFF `
  -Denable-pipewire=OFF `
  -Denable-readline=OFF `
  -Denable-threads=OFF `
  -Denable-openmp=OFF `
  -Denable-unicode=OFF `
  -Denable-native-dls=OFF `
  -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++"
```

```powershell
cmake --build build/windows-x86_64 --target install
```

## Android

```powershell
$NDK = "$env:LOCALAPPDATA\Android\Sdk\ndk\28.1.13356709"
```

```powershell
$env:ANDROID_HOME = "$env:LOCALAPPDATA\Android\Sdk"
```

### arm64-v8a

```powershell
cmake -B build/android-arm64-v8a -G "Ninja" `
  -DCMAKE_INSTALL_PREFIX="./install/android-arm64-v8a" `
  -Dosal=cpp11 `
  -DCMAKE_BUILD_TYPE=Release `
  -Denable-alsa=OFF `
  -Denable-aufile=OFF `
  -DBUILD_SHARED_LIBS=ON `
  -Denable-dbus=OFF `
  -Denable-ipv6=OFF `
  -Denable-jack=OFF `
  -Denable-ladspa=OFF `
  -Denable-libinstpatch=OFF `
  -Denable-libsndfile=OFF `
  -Denable-midishare=OFF `
  -Denable-opensles=OFF `
  -Denable-oboe=OFF `
  -Denable-network=OFF `
  -Denable-oss=OFF `
  -Denable-dsound=OFF `
  -Denable-wasapi=OFF `
  -Denable-waveout=OFF `
  -Denable-winmidi=OFF `
  -Denable-sdl3=OFF `
  -Denable-pulseaudio=OFF `
  -Denable-pipewire=OFF `
  -Denable-readline=OFF `
  -Denable-threads=OFF `
  -Denable-openmp=OFF `
  -Denable-unicode=OFF `
  -Denable-native-dls=OFF `
  -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" `
  -DANDROID_ABI=arm64-v8a `
  -DANDROID_PLATFORM=android-21 `
  -DANDROID_STL=c++_static
```

```powershell
cmake --build build/android-arm64-v8a --target install
```

### x86_64

```powershell
cmake -B build/android-x86_64 -G "Ninja" `
  -DCMAKE_INSTALL_PREFIX="./install/android-x86_64" `
  -Dosal=cpp11 `
  -DCMAKE_BUILD_TYPE=Release `
  -Denable-alsa=OFF `
  -Denable-aufile=OFF `
  -DBUILD_SHARED_LIBS=ON `
  -Denable-dbus=OFF `
  -Denable-ipv6=OFF `
  -Denable-jack=OFF `
  -Denable-ladspa=OFF `
  -Denable-libinstpatch=OFF `
  -Denable-libsndfile=OFF `
  -Denable-midishare=OFF `
  -Denable-opensles=OFF `
  -Denable-oboe=OFF `
  -Denable-network=OFF `
  -Denable-oss=OFF `
  -Denable-dsound=OFF `
  -Denable-wasapi=OFF `
  -Denable-waveout=OFF `
  -Denable-winmidi=OFF `
  -Denable-sdl3=OFF `
  -Denable-pulseaudio=OFF `
  -Denable-pipewire=OFF `
  -Denable-readline=OFF `
  -Denable-threads=OFF `
  -Denable-openmp=OFF `
  -Denable-unicode=OFF `
  -Denable-native-dls=OFF `
  -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" `
  -DANDROID_ABI=x86_64 `
  -DANDROID_PLATFORM=android-21 `
  -DANDROID_STL=c++_static `
  -DCMAKE_C_FLAGS="-mno-avx -mno-avx2" `
  -DCMAKE_CXX_FLAGS="-mno-avx -mno-avx2"
```

```powershell
cmake --build build/android-x86_64 --target install
```
