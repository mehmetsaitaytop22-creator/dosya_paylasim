# --- 1. Proje ve Derleyici Ayarları ---

# Minimum CMake versiyonunu belirt
cmake_minimum_required(VERSION 3.15)

# Proje adı. C++ testleri yazdığımız için CXX dilini de ekliyoruz.
project(WeatherStation C CXX) 

# C ve C++ standartlarını ayarla
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17) # Testler C++ olduğu için

# UTF-8 karakter setlerini zorunlu kıl (iyi bir pratiktir)
add_compile_options(-finput-charset=UTF-8)
add_compile_options(-fexec-charset=UTF-8)


# --- 2. Kendi C Kütüphanemiz (Uygulama ve Testlerin Ortak Kodu) ---

# lib/ klasöründeki tüm .c dosyalarını bul
file(GLOB LIB_SOURCES "lib/*.c")

# Bu .c dosyalarından 'weather_lib' adında bir kütüphane oluştur.
# Bu sayede aynı C kodlarını hem ana uygulamada hem de testlerde
# tekrar derlemeden ortak kullanabiliriz.
add_library(weather_lib ${LIB_SOURCES})

# Bu kütüphaneyi kullanan her projenin (veya testin)
# 'include' klasöründeki header (.h) dosyalarını
# otomatik olarak görmesini sağla.
target_include_directories(weather_lib PUBLIC include lib)

# --- 3. Ana Uygulama (weather_app) ---

# Curl kütüphanesini CMake'in kendi sistemiyle bul.
# (Sizin elle /mingw64/lib yazmanız yerine bu yöntem kullanılır)
find_package(CURL REQUIRED)

# Ana uygulamayı (executable) oluştur
add_executable(weather_app app/main.c)

# Ana uygulamayı hem kendi kütüphanemize (weather_lib) 
# hem de CMake'in bulduğu curl kütüphanesine (CURL::libcurl) bağla.
target_link_libraries(weather_app PRIVATE 
    weather_lib 
    CURL::libcurl
)


# --- 4. GoogleTest ile Test Kurulumu ---

# CTest (test çalıştırma) özelliğini projede aktifleştir
enable_testing()

# GoogleTest (gtest) kütüphanesini bul
find_package(GTest REQUIRED)

# Yüklediğiniz tüm test .cpp dosyalarından 
# 'run_tests' adında tek bir test programı oluştur.
# --- 4. GoogleTest ile Test Kurulumu --- (Değişiklik burada)

# ... (find_package(GTest REQUIRED) satırı kalsın) ...

# 'test/' klasöründeki BÜTÜN .cpp dosyalarını bul ve TEST_SOURCES değişkenine ata
file(GLOB TEST_SOURCES "test/*.cpp")

# Yüklediğiniz tüm test .cpp dosyalarından 
# 'run_tests' adında tek bir test programı oluştur.
add_executable(run_tests
    ${TEST_SOURCES}
)

# 'run_tests' programını hem kendi C kütüphanemize (weather_lib)
# hem de GoogleTest kütüphanesine (GTest::gtest_main) bağla.
target_link_libraries(run_tests PRIVATE 
    weather_lib 
    GTest::gtest_main
)

# GoogleTest'in CTest ile entegrasyonu için bu modülü dahil et
include(GoogleTest)

# 'run_tests' içindeki tüm TEST_F ve TEST makrolarını bulup
# CTest'e otomatik olarak ekle.
gtest_add_tests(TARGET run_tests)