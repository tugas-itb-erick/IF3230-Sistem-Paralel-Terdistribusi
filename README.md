# OpenMPI - Bitonic Sort
IF3230 - Sistem Paralel dan Terdistribusi
Erick Wijaya - 13515057

## Deskripsi Singkat
Program ini adalah ekstensi dari program bitonic sort yang disediakan oleh asisten. 
Program ini dapat mengeksekusi bitonic sort baik secara paralel. 
Setelah sorting dilakukan, program akan mencetak waktu eksekusi 
yang dibutuhkan untuk melakukan sort secara paralel. 

## Penggunaan Program
1. Lakukan kompilasi program dengan perintah ```make```. Pastikan sudah melakukan installasi OpenMPI sebelumnya. 
2. Jalankan program dengan perintah ```mpirun -np p ./bitonic_sort n```, dengan ```n``` adalah ukuran array dan ```p``` adalah jumlah proses.
3. Hasil array sebelum dan sesudah di-sort dapat dilihat pada file ```data/input.txt``` dan ```data/output.txt```
4. Output waktu eksekusi serial dan paralel dapat dilihat pada file ```output/log.txt```

## Pembagian Tugas
Tugas ini dikerjakan oleh Erick Wijaya (13515057). 

## Laporan Pengujian
Pada bagian ini akan dijelaskan mengenai deskripsi solusi paralel yang digunakan, analisis solusi, jumlah thread, pengujian, dan analisis hasil uji. 

### Deskripsi Solusi Paralel
Sebelum dijelaskan mengenai deskripsi solusi paralel akan dijelaskan terlebih dulu mengenai deskripsi solusi bitonic sort secara umum.
Bitonic sort adalah algoritma sorting yang berjalan dengan melakukan penukaran elemen untuk membuat *bitonic sequence*, yaitu deret angka 
yang nilainya naik lalu turun, atau turun lalu naik. Proses urutan penukaran (compare) dapat dilihat pada gambar dibawah. Panah 
menunjukkan parameter penukaran elemen yaitu elemen yang ditunjuk panah harus lebih besar atau sama dengan elemen yang menunjuk. 
Bila tidak demikian, posisi kedua elemen akan ditukar. Pada iterasi pertama, dihasilkan deretan angka yang naik-turun secara berselingan. 
Pada iterasi kedua dan ketiga, elemen yang ditukar diubah sehingga hasilnya adalah deretan angka naik-turun yang frekuensi naik-turunnya (daerah berwarna biru dan hijau pada gambar) adalah
setengahnya dari iterasi pertama. Hal tersebut dilakukan berulang sampai seluruh elemen hanya naik saja, atau turun saja, dalam kata lain 
sudah terurut menaik atau menurun (tergantung parameter sort ascending atau descending). 

![bitonic_sort](img/bitonic_sort.png)

Solusi diatas memiliki kompleksitas sebesar ```O(N*(log N)^2)```. Kompleksitas tersebut dapat dioptimasi dengan solusi paralel. 
Karena urutan penukaran elemen pada bitonic sort selalu sama diberikan ukuran elemen yang tetap, maka algoritma paralel dapat 
diimplementasikan dengan mudah. Solusi paralel diterapkan pada operasi penukaran (compare). Contohnya, pada iterasi pertama
di gambar terjadi 8 penukaran. Apabila dialokasikan 4 thread lalu menggunakan perintah ```#pragma omp parallel for```, maka 
setiap thread akan mengeksekusi 2 penukaran pada iterasi pertama. Solusi yang saya gunakan adalah dengan menggunakan perintah 
```#pragma omp parallel for num_threads(num_thread) shared(arr,j,k,N) private(i)``` persis sebelum melakukan iterasi untuk 
melakukan compare dan exchange sehingga setiap thread akan mengerjakan compare dan exchange secara merata. Parameter ```shared(arr,j,k,N)``` 
menunjukkan bahwa variabel tersebut digunakan oleh semua thread sedangkan parameter ```private(i)``` menunjukkan bahwa variabel i 
dimiliki oleh masing-masing thread.

### Analisis Solusi
Berdasarkan solusi yang saya gunakan, waktu eksekusi bitonic sort menjadi lebih cepat dengan speedup sekitar 2 kali. 

Karena paralelisasi dilakukan pada iterasi 
penukaran elemen (kompleksitasnya adalah ```O(N)```), kompleksitas iterasi tersebut menjadi lebih cepat, yaitu menjadi 
```O(N/K)``` dengan K adalah jumlah thread yang digunakan. Kompleksitas akhir algoritma menjadi ```O(N/K*(log N)^2)```. 
Solusi ini sudah cukup baik, namun bisa ditingkatkan lagi apabila ditemukan cara paralelisasi yang lebih efektif 
(misalnya dengan memparalelisasi looping pertama dan kedua dengan benar). 

Cara lain untuk mengoptimasi adalah dengan merancang algoritma bitonic sort 
yang tidak perlu mengalokasikan dan memproses elemen dummy bila ukuran array bukan kelipatan 2 (pada solusi yang saya gunakan masih 
menggunakan elemen dummy). 

### Jumlah Thread
Jumlah thread/proses yang digunakan adalah 2. Angka 2 dipilih karena berdasarkan hasil percobaan, program dengan 2 buah 
proses memiliki efisiensi lebih tinggi ketimbang dengan program yang memiliki 4 atau 8 proses atau lebih. 

Apabila yang ingin dikejar hanyalah *speedup* (tanpa memerhatikan efisiensi), program dengan 4 proses lebih baik karena 
memiliki *speedup* yang sedikit lebih tinggi dari program dengan 2 proses. 

### Pengukuran Kinerja (Tabel)
Berikut adalah tabel pengujian waktu untuk bitonic sort serial dan paralel.

#### Serial (1 proses)
| **Ukuran Array** | **Percobaan 1 (μs)** | **Percobaan 2 (μs)** | **Percobaan 3 (μs)** | **Percobaan 4 (μs)** | **Percobaan 5 (μs)** | **Rata-Rata (μs)** |
| ------------ | ------------------------ | -------------------- | -------------------- | -------------------- | -------------------- | ------------------ |
| 5000   | 2779.006958   | 2769.947052   | 2842.903137   | 2811.908722   | 2795.934677   | 2799.940109   |
| 50000  | 44410.943985  | 43387.889862  | 47386.884689  | 48340.082169  | 48374.891281  | 46380.138397  |
| 100000 | 101613.044739 | 107417.106628 | 108871.936798 | 107506.990433 | 92660.903931  | 103613.996506 |
| 200000 | 194581.031799 | 197408.914566 | 199373.006821 | 192059.040070 | 196392.059326 | 195962.810516 |
| 400000 | 366327.047348 | 377748.012543 | 372205.018997 | 371944.904327 | 364811.897278 | 370607.376099 |

#### Paralel (2 proses) 

#### Paralel (4 proses) 

#### Paralel (8 proses) 

Berikut adalah tabel speedup dan efisiensi dengan menggunakan 4 thread. 

| **Ukuran Array** | **Speed Up** | **Efisiensi** |
| ------------ | --- | --- |
| 5000         |  0.797x   |  19.917%   |
| 50000        |  1.701x   |  42.530%   |
| 100000       |  1.732x   |  43.321%   |
| 200000       |  1.860x   |  46.510%   |
| 400000       |  1.892x   |  47.302%   |

### Analisis Kinerja Serial dan Paralel
Berdasarkan pengukuran kinerja pada poin sebelumnya, terlihat bahwa kinerja program paralel lebih cepat daripada serial, 
kecuali untuk kasus ukuran array 5000. Pada kasus array 5000, program serial lebih cepat karena program paralel membutuhkan waktu 
tambahan untuk melakukan context switching. Semakin besar jumlah thread, semakin banyak context switching yang terjadi. Akan tetapi, 
apabila menggunakan 2 thread, program paralel cenderung lebih cepat karena jumlah context switching lebih kecil. Namun, untuk program ini
saya tetap menggunakan 4 thread karena untuk kasus array yang 
berukuran lebih besar, bitonic sort dengan 4 thread bekerja lebih cepat ketimbang dengan 2 thread.

Apabila dilihat dari tabel, efisiensi maupun speedup kinerja bitonic sort paralel dengan serial menjadi cenderung semakin tinggi. Hal tersebut demikian karena waktu untuk context switching menjadi tidak signifikan apabila 
ukuran permasalahan (array) lebih besar. Kesimpulannya, bitonic sort paralel memiliki performa yang lebih baik daripada 
bitonic sort serial kecuali untuk kasus persoalan yang kecil. Semakin besar jumlah thread, semakin cepat performa program paralel 
tetapi dibutuhkan waktu context switching yang bisa menjadi signifikan untuk kasus persoalan yang kecil. Jumlah thread yang melebihi 
jumlah core tidak akan membuat program paralel menjadi semakin cepat dari program dengan jumlah thread sejumlah core. 
