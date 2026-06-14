# STM32 Otonom Araç (İDA) Görev Kontrol ve Durum Makinesi

Bu proje, STM32 mikrodenetleyicileri üzerinde tamamen **Engellemesiz (Non-Blocking)** bir mimari ile çalışan, endüstriyel standartlarda güvenlik mekanizmalarına sahip bir otonom araç (İnsansız Deniz/Hava Aracı) simülasyonudur.

Sistem; sensör verilerini veya operatör komutlarını asenkron olarak işler, donanımsal kesmeler (Interrupt/DMA) aracılığıyla karar ağacında (State Machine) ilerler ve sistemin kilitlenmesi ihtimaline karşı **Donanımsal Bekçi Köpeği (IWDG)** ile kendi kendini kurtarabilme yeteneğine sahiptir.

## 🌟 Endüstriyel Mühendislik Yaklaşımları

Bu projede, gömülü sistem mimarisinde stabilite ve işlemci verimliliğini sağlamak için aşağıdaki ileri düzey teknikler kullanılmıştır:

* **FSM (Sonlu Durum Makinesi):** Sistemin görev adımları (Keşif, Kaza, Yakıt/Batarya Durumu, Eve Dönüş) karmaşık `if-else` spagettileri yerine, genişletilebilir bir `enum` ve `switch-case` mimarisi ile yönetilmiştir.
* **DMA Tabanlı Asenkron UART:** Kullanıcı/Operatör komutları, işlemciyi meşgul eden `HAL_UART_Receive_IT` veya Polling yöntemleri yerine, arka planda doğrudan belleğe yazan `HAL_UARTEx_ReceiveToIdle_DMA` ile okunur. Komutlar (\n, \r gibi) filtrelenerek sadece geçerli sinyaller işlenir.
* **Watchdog (IWDG) ile Çökme Koruması:** Sistem döngüsü sağlıklı çalıştığı sürece Watchdog beslenir. Beklenmedik bir kilitlenme veya sonsuz döngü durumunda donanım otomatik olarak güvenli bir **Reset** atar.
* **Donanımsal PWM ve Zamanlayıcılar:** Farklı uyarı durumları için (Kaza, Yakıt Azalması vb.) ana döngüyü yormadan çalışan "Nefes Alan LED" (Breathing) ve "Çakar LED" (Strobe) efektleri TIM1 üzerinden PWM kullanılarak tasarlanmıştır.
* **EXTI ve Yazılımsal Debounce:** Acil durum (Emergency) butonu dış kesme olarak ayarlanmış olup, elektriksel sıçramaları (bouncing) önlemek için kesme içerisinde `HAL_GetTick()` tabanlı **200ms'lik yazılımsal filtre** uygulanmıştır.

## ⚙️ Otonom Görev Senaryosu (Karar Ağacı)

Kullanıcı, seri haberleşme (UART) terminali üzerinden **'a', 'b', 'c'** komutları göndererek sistemin karar mekanizmasını test eder. Sistem aşağıdaki gibi dinamik senaryolara tepki verir:

1. **Keşif ve Başlangıç:** Rotanın onaylanması ve görevin başlaması.
2. **Kriz Yönetimi (Kaza Modu):** Motor arızası/yakıt kaçağı simülasyonunda sistem Çakar LED moduna geçer. Operatörden Mazot veya Batarya arasında güç seçimi yapması istenir.
3. **Kritik Batarya (Risk Algoritması):** Görev bitimine yakın enerji azalırsa sistem riskli kararlar (GPS iptali, yeryüzüne çıkma) sunar.
4. **Acil Durum (Eve Dön):** Operatör herhangi bir anda **EMG Butonuna** bastığında, görev derhal iptal edilir ve sistem tüm gücü "Eve Dönüş" (Return to Home) moduna aktarır.

## 📌 Donanım ve Pin Konfigürasyonu

* **Mikrodenetleyici:** STM32 Ailesi (Kod mimarisi taşınabilir şekilde HAL kütüphanesiyle yazılmıştır).
* **Seri Haberleşme (UART1):** `115200 Baud Rate`, `8 Data Bits`, `No Parity`, `1 Stop Bit`.
* **Acil Durum Butonu (EXTI):** `emg_Pin` (Yükselen Kenar Kesmesi).
* **Uyarı LED'leri (TIM1 PWM):** `TIM_CHANNEL_1` (Çakar), `TIM_CHANNEL_3` (Nefes Alan).
* **Sistem Güvenliği:** Dahili LSI Osilatör ile beslenen Independent Watchdog (IWDG).

## 🚀 Kurulum ve Test (UART Üzerinden)

1. Projeyi bilgisayarınıza klonlayın:
   ```bash
   git clone [https://github.com/KULLANICI_ADINIZ/STM32-Autonomous-FSM-Rover.git](https://github.com/KULLANICI_ADINIZ/STM32-Autonomous-FSM-Rover.git)
