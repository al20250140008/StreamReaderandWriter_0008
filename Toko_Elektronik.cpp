#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <algorithm>

//  Struktur data satu produk
struct Produk {
    int         id;
    std::string nama;
    std::string kategori;
    double      harga;
    int         stok;
};


//  Helper: format angka ribuan (Rp)
std::string formatRupiah(double nominal) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << nominal;
    std::string angka = oss.str();
    int insertPos = static_cast<int>(angka.size()) - 3;
    while (insertPos > 0) {
        angka.insert(insertPos, ".");
        insertPos -= 3;
    }
    return "Rp " + angka;
}
//  Warna ANSI (opsional, aman di terminal)

namespace Warna {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string MERAH   = "\033[31m";
    const std::string HIJAU   = "\033[32m";
    const std::string KUNING  = "\033[33m";
    const std::string BIRU    = "\033[34m";
    const std::string CYAN    = "\033[36m";
}

//  KELAS  : TokoElektronik
//  Tujuan : Merepresentasikan etalase fisik (3 rak)
//           dengan OOP + Exception Handling
class TokoElektronik {
private:
    // Atribut private — array 3 elemen (enkapsulasi)
    std::array<Produk, 3> etalase;

public:
    // Constructor: isi data awal etalase secara otomatis
    TokoElektronik() {
        etalase[0] = {1, "Samsung Galaxy S24",    "Smartphone",  14999000, 5};
        etalase[1] = {2, "MacBook Air M3",         "Laptop",      18500000, 3};
        etalase[2] = {3, "Sony WH-1000XM5",        "Headphone",    4750000, 8};
    }

    // Method public: ambil produk berdasarkan nomorRak
    Produk ambilProduk(size_t nomorRak) {
        try {
            // Wajib menggunakan .at() agar melempar out_of_range otomatis
            return etalase.at(nomorRak);
        }
        catch (const std::out_of_range&) {
            // Tangkap lalu lempar pesan kustom
            throw std::runtime_error(
                "Gagal Mengambil Barang : Rak nomor " +
                std::to_string(nomorRak) +
                " kosong atau tidak tersedia!"
            );
        }
    }

    // Tampilkan seluruh isi etalase
    void tampilkanEtalase() const {
        std::cout << Warna::CYAN << Warna::BOLD
                  << "\n======================================================"
                  << "\n         ETALASE TOKO ELEKTRONIK \"GIBRAN JAYA\"      "
                  << "\n======================================================"
                  << Warna::RESET << "\n";
        std::cout << std::left
                  << std::setw(5)  << "Rak"
                  << std::setw(25) << "Nama Produk"
                  << std::setw(14) << "Kategori"
                  << std::setw(18) << "Harga"
                  << std::setw(6)  << "Stok"
                  << "\n"
                  << std::string(68, '-') << "\n";

        for (size_t i = 0; i < etalase.size(); ++i) {
            const Produk& p = etalase[i];
            std::cout << std::left
                      << std::setw(5)  << i
                      << std::setw(25) << p.nama
                      << std::setw(14) << p.kategori
                      << std::setw(18) << formatRupiah(p.harga)
                      << std::setw(6)  << p.stok
                      << "\n";
        }
        std::cout << std::string(68, '-') << "\n";
    }
};

//  KELAS  : GudangManager
//  Tujuan : CRUD berbasis file (gudang.txt)

class GudangManager {
private:
    const std::string namaFile = "gudang.txt";

    // ── Utilitas: tulis semua produk ke file ──────────────────
    void tulisFile(const std::vector<Produk>& daftar) const {
        std::ofstream file(namaFile, std::ios::trunc);
        if (!file.is_open())
            throw std::runtime_error("Tidak dapat membuka file: " + namaFile);

        for (const Produk& p : daftar) {
            file << p.id      << "|"
                 << p.nama    << "|"
                 << p.kategori<< "|"
                 << p.harga   << "|"
                 << p.stok    << "\n";
        }
    }

    // ── Utilitas: baca semua produk dari file ─────────────────
    std::vector<Produk> bacaFile() const {
        std::vector<Produk> daftar;
        std::ifstream file(namaFile);
        if (!file.is_open()) return daftar; // file belum ada, kembalikan kosong

        std::string baris;
        while (std::getline(file, baris)) {
            if (baris.empty()) continue;
            std::istringstream ss(baris);
            std::string token;
            Produk p;
            try {
                std::getline(ss, token, '|'); p.id       = std::stoi(token);
                std::getline(ss, token, '|'); p.nama     = token;
                std::getline(ss, token, '|'); p.kategori = token;
                std::getline(ss, token, '|'); p.harga    = std::stod(token);
                std::getline(ss, token, '|'); p.stok     = std::stoi(token);
                daftar.push_back(p);
            }
            catch (...) { /* baris korup, lewati */ }
        }
        return daftar;
    }

    // ── Utilitas: cari ID tertinggi ───────────────────────────
    int idTertinggi(const std::vector<Produk>& daftar) const {
        int maks = 0;
        for (const Produk& p : daftar)
            if (p.id > maks) maks = p.id;
        return maks;
    }

    // ── Utilitas: input string tidak kosong ───────────────────
    std::string inputString(const std::string& prompt) const {
        std::string hasil;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, hasil);
            if (!hasil.empty()) return hasil;
            std::cout << Warna::KUNING << "  Input tidak boleh kosong!\n" << Warna::RESET;
        }
    }

    // ── Utilitas: input angka positif 
    double inputDouble(const std::string& prompt) const {
        double nilai;
        while (true) {
            std::cout << prompt;
            if (std::cin >> nilai && nilai >= 0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return nilai;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << Warna::KUNING << "  Masukkan angka positif yang valid!\n" << Warna::RESET;
        }
    }

    int inputInt(const std::string& prompt) const {
        return static_cast<int>(inputDouble(prompt));
    }

public:
    // ── CREATE 
    void tambahBarang() {
        std::cout << Warna::HIJAU << Warna::BOLD
                  << "\n─── TAMBAH BARANG BARU ───────────────────────\n"
                  << Warna::RESET;

        std::vector<Produk> daftar = bacaFile();
        Produk p;
        p.id       = idTertinggi(daftar) + 1;
        p.nama     = inputString("  Nama Produk    : ");
        p.kategori = inputString("  Kategori       : ");
        p.harga    = inputDouble("  Harga (Rp)     : ");
        p.stok     = inputInt   ("  Stok (unit)    : ");

        daftar.push_back(p);
        tulisFile(daftar);

        std::cout << Warna::HIJAU
                  << "  ✔ Barang \"" << p.nama << "\" berhasil ditambahkan (ID: "
                  << p.id << ").\n" << Warna::RESET;
    }

    // ── READ 
    void tampilkanBarang() const {
        std::vector<Produk> daftar = bacaFile();

        std::cout << Warna::BIRU << Warna::BOLD
                  << "\n======================================================"
                  << "\n              DAFTAR BARANG GUDANG \"GIBRAN JAYA\"            "
                  << "\n======================================================"
                  << Warna::RESET << "\n";

        if (daftar.empty()) {
            std::cout << Warna::KUNING << "  Gudang masih kosong. Silakan tambah barang terlebih dahulu.\n"
                      << Warna::RESET;
            return;
        }

        std::cout << std::left
                  << std::setw(5)  << "ID"
                  << std::setw(28) << "Nama Produk"
                  << std::setw(16) << "Kategori"
                  << std::setw(18) << "Harga"
                  << std::setw(6)  << "Stok"
                  << "\n"
                  << std::string(73, '-') << "\n";

        for (const Produk& p : daftar) {
            std::cout << std::left
                      << std::setw(5)  << p.id
                      << std::setw(28) << p.nama
                      << std::setw(16) << p.kategori
                      << std::setw(18) << formatRupiah(p.harga)
                      << std::setw(6)  << p.stok
                      << "\n";
        }
        std::cout << std::string(73, '-') << "\n"
                  << "  Total: " << daftar.size() << " produk\n";
    }

    // ── UPDATE 
    void updateBarang() {
        tampilkanBarang();
        std::vector<Produk> daftar = bacaFile();
        if (daftar.empty()) return;

        std::cout << Warna::KUNING << Warna::BOLD
                  << "\n┌─── UPDATE BARANG ────────────────────────────┐\n"
                  << Warna::RESET;

        int cariId = inputInt("  Masukkan ID produk yang ingin diubah : ");

        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [cariId](const Produk& p){ return p.id == cariId; });

        if (it == daftar.end()) {
            std::cout << Warna::MERAH << "  ✘ Produk dengan ID " << cariId << " tidak ditemukan.\n"
                      << Warna::RESET;
            return;
        }

        std::cout << "  Produk ditemukan: " << Warna::CYAN << it->nama << Warna::RESET << "\n"
                  << "  (Kosongkan Enter untuk tidak mengubah)\n\n";

        auto tanyaAtau = [&](const std::string& label, const std::string& nilai) -> std::string {
            std::cout << "  " << label << " [" << nilai << "] : ";
            std::string masukan;
            std::getline(std::cin, masukan);
            return masukan.empty() ? nilai : masukan;
        };

        it->nama      = tanyaAtau("Nama Produk   ", it->nama);
        it->kategori  = tanyaAtau("Kategori      ", it->kategori);

        std::cout << "  Harga [" << formatRupiah(it->harga) << "] : ";
        std::string hStr; std::getline(std::cin, hStr);
        if (!hStr.empty()) { try { it->harga = std::stod(hStr); } catch(...){} }

        std::cout << "  Stok [" << it->stok << "] : ";
        std::string sStr; std::getline(std::cin, sStr);
        if (!sStr.empty()) { try { it->stok = std::stoi(sStr); } catch(...){} }

        tulisFile(daftar);
        std::cout << Warna::HIJAU << "  ✔ Data produk ID " << cariId << " berhasil diperbarui.\n"
                  << Warna::RESET;
    }

    // ── DELETE 
    void hapusBarang() {
        tampilkanBarang();
        std::vector<Produk> daftar = bacaFile();
        if (daftar.empty()) return;

        std::cout << Warna::MERAH << Warna::BOLD
                  << "\n─── HAPUS BARANG ─────────────────────────────\n"
                  << Warna::RESET;

        int cariId = inputInt("  Masukkan ID produk yang ingin dihapus : ");

        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [cariId](const Produk& p){ return p.id == cariId; });

        if (it == daftar.end()) {
            std::cout << Warna::MERAH << "  ✘ Produk dengan ID " << cariId << " tidak ditemukan.\n"
                      << Warna::RESET;
            return;
        }

        std::string konfirmasi;
        std::cout << "  Yakin hapus \"" << it->nama << "\"? (y/n) : ";
        std::getline(std::cin, konfirmasi);
        if (konfirmasi != "y" && konfirmasi != "Y") {
            std::cout << "  Penghapusan dibatalkan.\n";
            return;
        }

        std::string namaTerhapus = it->nama;
        daftar.erase(it);
        tulisFile(daftar);
        std::cout << Warna::HIJAU << "  ✔ Produk \"" << namaTerhapus << "\" berhasil dihapus.\n"
                  << Warna::RESET;
    }
};

//  SIMULASI ETALASE — menguji exception handling

void simulasiEtalase() {
    TokoElektronik toko;
    toko.tampilkanEtalase();

    std::cout << Warna::BOLD
              << "\n━━━ SIMULASI PENGAMBILAN BARANG ━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
              << Warna::RESET;

    // Skenario 1 — indeks 1 → harus SUKSES
    std::cout << Warna::CYAN << "▶ Skenario 1: Ambil barang di Rak indeks ke-1\n" << Warna::RESET;
    try {
        Produk p = toko.ambilProduk(1);
        std::cout << Warna::HIJAU
                  << "  ✔ Berhasil! Produk ditemukan:\n"
                  << "    Nama     : " << p.nama      << "\n"
                  << "    Kategori : " << p.kategori  << "\n"
                  << "    Harga    : " << formatRupiah(p.harga) << "\n"
                  << "    Stok     : " << p.stok      << " unit\n"
                  << Warna::RESET;
    }
    catch (const std::runtime_error& e) {
        std::cout << Warna::MERAH << "  ✘ Error: " << e.what() << "\n" << Warna::RESET;
    }

    std::cout << "\n";

    // Skenario 2 — indeks 5 → harus GAGAL dengan pesan kustom
    std::cout << Warna::CYAN << "▶ Skenario 2: Ambil barang di Rak indeks ke-5\n" << Warna::RESET;
    try {
        Produk p = toko.ambilProduk(5);
        std::cout << Warna::HIJAU << "  ✔ Berhasil! Produk: " << p.nama << "\n" << Warna::RESET;
    }
    catch (const std::runtime_error& e) {
        std::cout << Warna::MERAH << "  ✘ Error: " << e.what() << "\n" << Warna::RESET;
    }

    std::cout << Warna::BOLD
              << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
              << Warna::RESET;
}

//  MENU UTAMA
void tampilkanMenu() {
    std::cout << Warna::BOLD << Warna::BIRU
              << "\n======================================================"
              << "\n     SISTEM MANAJEMEN TOKO ELEKTRONIK         "
              << "\n              \"GIBRAN JAYA\"                 "
              << "\n======================================================\n"
              << Warna::RESET
              << "  " << Warna::HIJAU  << "[1]" << Warna::RESET << " Tampilkan Daftar Barang Gudang\n"
              << "  " << Warna::HIJAU  << "[2]" << Warna::RESET << " Tambah Barang Baru\n"
              << "  " << Warna::KUNING << "[3]" << Warna::RESET << " Update Data Barang\n"
              << "  " << Warna::MERAH  << "[4]" << Warna::RESET << " Hapus Barang\n"
              << "  " << Warna::CYAN   << "[5]" << Warna::RESET << " Simulasi Etalase (Exception Handling)\n"
              << "  " << Warna::BOLD   << "[0]" << Warna::RESET << " Keluar\n"
              << Warna::BIRU
              << "======================================================\n"
              << Warna::RESET
              << "  Pilihan Anda : ";
}

//  FUNGSI MAIN

int main() {
    GudangManager gudang;
    int pilihan = -1;

    std::cout << Warna::BOLD << Warna::CYAN
              << "\n  Selamat Datang di Sistem Manajemen\n"
              << "  Toko Elektronik \"Gibran Jaya\"\n"
              << Warna::RESET;

    do {
        tampilkanMenu();

        // Baca pilihan dengan validasi
        if (!(std::cin >> pilihan)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            pilihan = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (pilihan) {
            case 1: gudang.tampilkanBarang(); break;
            case 2: gudang.tambahBarang();    break;
            case 3: gudang.updateBarang();    break;
            case 4: gudang.hapusBarang();     break;
            case 5: simulasiEtalase();        break;
            case 0:
                std::cout << Warna::HIJAU
                          << "\n  Terima kasih telah menggunakan sistem ini.\n"
                          << "  Sampai jumpa!\n\n"
                          << Warna::RESET;
                break;
            default:
                std::cout << Warna::KUNING
                          << "  Pilihan tidak valid. Silakan masukkan angka 0-5.\n"
                          << Warna::RESET;
        }

    } while (pilihan != 0);

    return 0;
}