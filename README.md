# I2C Veri Okuma ve Yazma Örneği

Bu örnek uygulama, Texas Instruments Tiva4C mikrodenetleyici kartı üzerinde bulunan bir I2C (Inter-Integrated Circuit) sensöründen veri okuma ve yazma işlemlerini gerçekleştirir.

## Donanım Bağlantıları

Bu örnekte, GY-521 adlı I2C sensörü, EK-TM4C1294-XL launchpad kartına bağlanır. Bağlantılar şu şekildedir:

- PD0 (launchpad) --> SCL (GY-521)
- PD1 (launchpad) --> SDA (GY-521)
- +3.3V (launchpad) --> GND (GY-521)
- GND (launchpad) --> VCC (GY-521)

## Kod İşlevi

Bu kod bloğu aşağıdaki işlevleri gerçekleştirir:

- I2C arayüzünü kullanarak belirli bir I2C cihazına veri yazma ve okuma işlemleri yapar.
- Belirli bir I2C cihazına veri yazmak için `IIC_writeReg` fonksiyonunu kullanır.
- Belirli bir I2C cihazından veri okumak için `IIC_readReg` fonksiyonunu kullanır.

## Kullanılan Kütüphaneler ve Araçlar

- TI-RTOS (TI Gerçek Zamanlı İşletim Sistemi)
- TI-DRIVERS Kütüphanesi
- Board.h: Kartın pin konfigürasyonunu ve özelliklerini tanımlar.
- GPIO.h: Genel amaçlı giriş/çıkış (GPIO) işlevlerini sağlar.
- I2C.h: I2C iletişim işlevlerini sağlar.
