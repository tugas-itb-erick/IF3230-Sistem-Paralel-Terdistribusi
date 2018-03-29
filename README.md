# CUDA - Bitonic Sort
IF3230 - Sistem Paralel dan Terdistribusi

Erick Wijaya - 13515057

## Deskripsi Singkat
Program ini adalah ekstensi dari program bitonic sort yang disediakan oleh asisten. 
Program ini dapat mengeksekusi bitonic sort secara paralel dengan memanfaatkan CUDA. 
Setelah sorting dilakukan, program akan mencetak waktu eksekusi 
yang dibutuhkan untuk melakukan sort secara paralel. 

## Penggunaan Program
1. Lakukan kompilasi program dengan perintah ```make```. Pastikan sudah melakukan installasi OpenMPI sebelumnya. 
2. Jalankan program dengan perintah ```,/bitonic_sort n p```, dengan ```n``` adalah ukuran array dan ```p``` adalah jumlah thread per block (optional).
3. Hasil array sebelum dan sesudah di-sort dapat dilihat pada file ```data/input.txt``` dan ```data/output.txt```
4. Output waktu eksekusi beserta detilnya dapat dilihat pada file ```output/log.txt```

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
diimplementasikan dengan mudah. 

Solusi paralel diterapkan dengan mengalokasikan proses sejumlah dengan ukuran array dengan menggunakan perintah ```cudaMalloc``` dan ```cudaMemcpy```. Masing-masing proses berperan untuk melakukan perbandingan 
dan penukaran elemen. Proses tersebut dibuat pada setiap iterasi dan mengalokasikan jumlah blocks dan threads dengan memanggil ```bitonicSortStep<<<blocks, threads>>>```. 
Setelah seluruh iterasi selesai, hasil pengurutan disalin ke array dengan ```cudaMemcpy``` dan didealokasi dengan perintah ```cudaFree```. 

### Analisis Solusi
Berdasarkan solusi yang saya gunakan, waktu eksekusi bitonic sort dapat mencapai *speedup* sebesar 18x untuk kasus ukuran array 8 juta.  

Karena paralelisasi dilakukan pada setiap iterasi 
penukaran elemen (kompleksitasnya adalah ```O(N)```), kompleksitas iterasi tersebut menjadi lebih cepat, yaitu menjadi 
```O(N/K)``` dengan K adalah jumlah proses. Kompleksitas algoritma menjadi ```O(N/K*(log N)^2)```. Akan tetapi, karena nilai K=N, maka 
kompleksitas algoritma menjadi ```O((log N)^2)```. 
Solusi ini sudah cukup baik, namun bisa ditingkatkan lagi apabila ditemukan cara paralelisasi yang lebih efektif 
(misalnya dengan memparalelisasi looping pertama dan kedua dengan benar). 

Cara lain untuk mengoptimasi adalah dengan merancang algoritma bitonic sort 
yang tidak perlu mengalokasikan dan memproses elemen dummy bila ukuran array bukan kelipatan 2. 

### Jumlah Thread
Jumlah proses yang digunakan adalah sama dengan ukuran array. Jumlah thread selalu 512 sedangkan jumlah block adalah ukuran array dibagi jumlah thread. 
Misalnya apabila ukuran array sebesar 4096, maka akan ada 4096 proses terpisah dengan jumlah block sebesar 8 dan jumlah thread sebesar 512. Jumlah 
proses disesuaikan dengan ukuran array karena CUDA sudah menyediakan struktur data untuk mendukung *multithreading* dengan proses yang sangat banyak. 

### Pengukuran Kinerja (Tabel)
Berikut adalah tabel pengujian waktu untuk bitonic sort serial dan paralel.

#### Serial
| **Ukuran Array** | **Percobaan 1 (μs)** | **Percobaan 2 (μs)** | **Percobaan 3 (μs)** | **Rata-Rata (μs)** |
| ---------------- | -------------------- | -------------------- | -------------------- | ------------------ |
| 512     | 525.951385 | 484.943390 | 494.956970 | 501.950582 |
| 1024    | 838.041306 | 958.919525 | 832.080841 | 876.347224 |
| 4096    | 4766.941071 | 4779.100418 | 4252.910614 | 4599.650701 |
| 65536   | 70756.912231 | 70929.050446 | 70732.116699 | 70806.026459 |
| 262144  | 352247.953415 | 352333.068848 | 353120.088577 | 352567.036947 |
| 1048576 | 1721033.096313 | 1721673.011780 | 1726861.000061 | 1723189.036051 |
| 8388608 | 18181659.936905 | 18317732.095718 | 18041908.025742 | 18180433.352788 |

#### Paralel
| **Ukuran Array** | **Percobaan 1 (μs)** | **Percobaan 2 (μs)** | **Percobaan 3 (μs)** | **Rata-Rata (μs)** | **Speed Up** |
| ---------------- | -------------------- | -------------------- | -------------------- | ------------------ | ------------ |
| 512     | 869102.001190 | 870419.979095 | 872874.975204 | 870798.985163 | 0.0005x |
| 1024    | 867660.045624 | 869436.025620 | 864880.084991 | 867325.385412 | 0.001x  |
| 4096    | 869415.044785 | 864294.052124 | 869880.914688 | 867863.337199 | 0.005x  |
| 65536   | 860658.884048 | 860471.963882 | 866123.914719 | 862418.254216 | 0.082x  |
| 262144  | 869347.095490 | 863545.894623 | 869842.052460 | 867578.347524 | 0.406x  |
| 1048576 | 882438.898087 | 878082.990646 | 874258.041382 | 878259.976705 | 1.962x  |
| 8388608 | 994366.884232 | 995826.005936 | 991144.895554 | 993779.261907 | 18.294x |

### Analisis Kinerja Serial dan Paralel
Berdasarkan hasil pengukuran kinerja diatas, terlihat bahwa kinerja program paralel jauh lebih lambat daripada serial untuk ukuran arrray 
yang kecil (512~262144). Namun ketika ukuran array mencapai 1 juta, program paralel lebih cepat dengan *speedup* hampir mencapai dua kali. 
Pada kasus ukuran array yang sangat besar (8 juta), program paralel memiliki *speedup* mencapai 18x. Pada kasus ukuran array yang kecil, program 
paralel lebih lambat karena overhead yang muncul (karena menggunakan banyak proses) lebih signifikan daripada waktu eksekusi untuk mengurutkan array. 
Akan tetapi, seiring bertambahnya ukuran array, overhead menjadi semakin tidak signifikan sehingga *speedup* menjadi lebih tinggi, hingga mencapai 
suatu titik dimana program paralel berjalan lebih cepat daripada program serial. Hal ini terlihat dari rata-rata waktu eksekusi yang tidak berbeda jauh 
untuk masing-masing kasus ukuran array. Pada kasus ukuran array 8 juta, program paralel jauh lebih cepat daripada serial karena overhead sudah sangat 
tidak signifikan, tetapi rata-rata waktunya meningkat dariada kasus sebelumnya karena jumlah proses sudah melebihi batas yang disediakan oleh CUDA. 
Kesimpulannya, paralelisasi dengan CUDA sangat efektif pada ukuran kasus yang sangat besar dan kurang efektif pada ukuran kasus yang kecil. 