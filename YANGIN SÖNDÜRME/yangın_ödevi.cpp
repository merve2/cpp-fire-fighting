#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const int MAX_SATIR = 100;
const int MAX_SUTUN = 100;
const int YAKIT_KAPASITE = 5000;
const int SU_TANK_KAPASITE = 20000;
const int HIZ = 1;
const int YAKIT_TUKETIMI = 10;

class Orman {
public:
    void ormanOku(const string& dosyaAdi);
    void suDolumu();
    void yakitDolumu();
    // Diğer fonksiyonlar ve veri elemanları eklenebilir

private:
    char ormanGrafi[MAX_SATIR][MAX_SUTUN];
    int satirSayisi;
    int sutunSayisi;
    int ucakKonum[2];
    bool suDepoDolu;
    bool yakitDepoDolu;
    int yanginSiddeti[MAX_SATIR][MAX_SUTUN];
};

void Orman::ormanOku(const string& dosyaAdi) {
    ifstream dosya(dosyaAdi);
    if (!dosya) {
        cerr << "Dosya acilamadi: " << dosyaAdi << endl;
        exit(EXIT_FAILURE);
    }

    dosya >> satirSayisi >> sutunSayisi;

    for (int i = 0; i < satirSayisi; ++i) {
        for (int j = 0; j < sutunSayisi; ++j) {
            dosya >> ormanGrafi[i][j];
            if (ormanGrafi[i][j] == 'B0') {
                ucakKonum[0] = i;
                ucakKonum[1] = j;
            }
            dosya >> yanginSiddeti[i][j];
        }
    }

    suDepoDolu = true;
    yakitDepoDolu = true;

    dosya.close();
}

void Orman::suDolumu() {
    cout << "Su deposu dolduruldu." << endl;
    suDepoDolu = true;
}

void Orman::yakitDolumu() {
    cout << "Yakit deposu dolduruldu." << endl;
    yakitDepoDolu = true;
}

class YanginSondurmeUcagi {
public:
    void yanginSondur(Orman& orman);
    // Diğer fonksiyonlar ve veri elemanları eklenebilir
private:
    int yakitKapasitesi;
    int suTankKapasitesi;
    int hiz;
    int yakitTuketimi;

    int yanginSondurmeSuresi(int yanginDerecesi);
    int suHarcamaMiktari(int yanginDerecesi);
};

void YanginSondurmeUcagi::yanginSondur(Orman& orman) {
    int hedefKonum[2] = { 0, 0 }; // Hedef başlangıçta ormanın sol üst köşesi
    int mesafe = 0;

    while (mesafe < orman.satirSayisi * orman.sutunSayisi) {
        // Hedefe ulaşılmadıysa devam et
        int deltaX = hedefKonum[0] - orman.ucakKonum[0];
        int deltaY = hedefKonum[1] - orman.ucakKonum[1];
        int uzaklik = abs(deltaX) + abs(deltaY);

        if (uzaklik == 0) {
            // Hedefe ulaşıldı, yangını söndür
            int yanginDerecesi = orman.yanginSiddeti[hedefKonum[0]][hedefKonum[1]];
            int sure = yanginSondurmeSuresi(yanginDerecesi);
            int suHarcama = suHarcamaMiktari(yanginDerecesi);

            // Havada geçen süreye göre yakıt tüketimi
            int yakitTuketimi = sure * YAKIT_TUKETIMI;
            if (yakitTuketimi <= yakitKapasitesi && suHarcama <= suTankKapasitesi) {
                // Yangını söndür ve kaynakları azalt
                cout << "Yangin sonduruldu. Sure: " << sure << " dakika. Su Harcama: " << suHarcama << " litre." << endl;

                // Su kullanımına bağlı olarak yangın şiddetini azalt
                int suKullanilanMiktar = suHarcama;
                while (suKullanilanMiktar >= 1000 && orman.yanginSiddeti[hedefKonum[0]][hedefKonum[1]] > 1) {
                    suKullanilanMiktar -= 1000;
                    orman.yanginSiddeti[hedefKonum[0]][hedefKonum[1]]--;
                }

                yakitKapasitesi -= yakitTuketimi;
                suTankKapasitesi -= suHarcama;
            }
            else {
                // Kaynaklar yetmiyorsa B0 konumuna dön ve kaynakları doldur
                orman.ucakKonum[0] = 0;
                orman.ucakKonum[1] = 0;
                cout << "Kaynaklar yetmiyor, B0 konumuna donuluyor." << endl;
                orman.yakitDolumu();
                orman.suDolumu();
            }

            break;
        }

        // Hedefe ulaşmak için gereken yakıt miktarı
        int gerekliYakit = uzaklik * YAKIT_TUKETIMI;

        if (gerekliYakit <= yakitKapasitesi) {
            // Hedefe ulaşmak için yeterli yakıt var
            yakitKapasitesi -= gerekliYakit;
            orman.ucakKonum[0] = hedefKonum[0];
            orman.ucakKonum[1] = hedefKonum[1];
            mesafe += uzaklik;
            cout << "Hedefe gidiliyor. Mesafe: " << mesafe << " km" << endl;
        }
        else {
            // Yakıt yetmiyorsa B0 konumuna dön ve yakıt doldur
            orman.ucakKonum[0] = 0;
            orman.ucakKonum[1] = 0;
            cout << "Yakıt yetmiyor, B0 konumuna donuluyor." << endl;
            orman.yakitDolumu();
        }
    }
}

int YanginSondurmeUcagi::yanginSondurmeSuresi(int yanginDerecesi) {
    // Yangın derecesine göre söndürme süresi
    switch (yanginDerecesi) {
    case 1: return 10;
    case 2: return 20;
    case 3: return 30;
    case 4: return 40;
    case 5: return 50;
    default: return 0;
    }
}

int YanginSondurmeUcagi::suHarcamaMiktari(int yanginDerecesi) {
    // Yangın derecesine göre harcanan su miktarı
    switch (yanginDerecesi) {
    case 1: return 1000;
    case 2: return 2000;
    case 3: return 3000;
    case 4: return 4000;
    case 5: return 5000;
    default: return 0;
    }
}

int main() {
    srand(time(NULL)); // Rastgele sayı üretimi için seed ayarı

    Orman orman;
    YanginSondurmeUcagi ucak;

    // Ormanı dosyadan oku
    orman.ormanOku("Orman.txt");

    // Yangın söndürme uçağı işlemleri
    ucak.yanginSondur(orman);

    return 0;
}
