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

Solusi paralel diterapkan dengan membagi array menjadi beberapa array kecil dengan ```MPI_Scatter``` sesuai dengan jumlah prosesnya. 
Masing-masing proses akan melakukan bitonic sort terhadap array kecil tersebut. Misalnya array memiliki ukuran 5000, apabila digunakan 
2 proses maka array dibagi menjadi 2 array berukuran 2500 sehingga masing-masing proses melakukan bitonic sort dengan ukuran 2500. 
Setelah semua proses melakukan bitonic sort, masing-masing proses bersebelahan melakukan komunikasi dengan ```MPI_Sendrecv``` untuk 
menggabungkan kedua array menjadi satu array gabungan bitonic sequence. Misalnya ada 4 proses, maka proses 0 dan 1 melakukan komunikasi 
sehingga array pada proses 0 dan 1 terurut bila digabung. Begitu pula dengan array pada proses 2 dan 3. Setelah itu, hasil array proses 0,1 
dan proses 2,3 dibandingkan untuk membentuk array dari proses 1,2,3,4 yang terurut sehingga seluruh array-array kecil sudah terurut bila 
digabungkan. Untuk menggabungkan semua array-array kecil, digunakan perintah ```MPI_Gather``` sehingga diperoleh sebuah array gabungan 
yang sudah terurut. Perintah ```MPI_Barrier``` dipanggil sebelum gather untuk menghindari penggabungan array kecil yang belum selesai 
diproses. 

### Analisis Solusi
Berdasarkan solusi yang saya gunakan, waktu eksekusi bitonic sort menjadi lebih cepat dengan speedup hampir ```x``` kali. 

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

### Pengukuran Kinerja (Tabel)
Berikut adalah tabel pengujian waktu untuk bitonic sort serial dan paralel, dan tabel *speedup* beserta efisiensinya.

#### Serial (1 proses)
| **Ukuran Array** | **Percobaan 1 (μs)** | **Percobaan 2 (μs)** | **Percobaan 3 (μs)** | **Rata-Rata (μs)** |
| ---------------- | -------------------- | -------------------- | -------------------- | ------------------ |
| 512     | 525.951385 | 484.943390 | 494.956970 |  |
| 1024    | 838.041306 | 958.919525 | 832.080841 |  |
| 4096    | 4766.941071 | 4779.100418 | 4252.910614 |  |
| 65536   | 70756.912231 | 70929.050446 | 70732.116699 |  |
| 262144  | 352247.953415 | 352333.068848 | 353120.088577 |  |
| 1048576 | 1721033.096313 | 1721673.011780 | 1726861.000061 |  |
| 8388608 | 18181659.936905 | 18317732.095718 | 18041908.025742 |  |

#### Paralel (n proses) 
| **Ukuran Array** | **Percobaan 1 (μs)** | **Percobaan 2 (μs)** | **Percobaan 3 (μs)** | **Rata-Rata (μs)** | **Speed Up** | **Efisiensi** |
| ---------------- | -------------------- | -------------------- | -------------------- | ------------------ | ------------ | ------------- |
| 512     | 869102.001190 | 870419.979095 | 872874.975204 |  |  |  |
| 1024    | 867660.045624 | 869436.025620 | 864880.084991 |  |  |  |
| 4096    | 869415.044785 | 864294.052124 | 869880.914688 |  |  |  |
| 65536   | 860658.884048 | 860471.963882 | 866123.914719 |  |  |  |
| 262144  | 869347.095490 | 863545.894623 | 869842.052460 |  |  |  |
| 1048576 | 882438.898087 | 878082.990646 | 874258.041382 |  |  |  |
| 8388608 | 994366.884232 | 995826.005936 | 991144.895554 |  |  |  |

### Analisis Kinerja Serial dan Paralel
Berdasarkan hasil pengukuran kinerja diatas, terlihat bahwa kinerja program paralel lebih cepat daripada serial untuk semua kasus uji. 
Hasil diatas menunjukkan bahwa program dengan 2 proses memiliki *speedup* dan efisiensi tertinggi. Hal tersebut demikian karena 
pada kasus 2 proses, *overhead* lebih sedikit karena lebih sedikit context switching dan komunikasi antar proses. 
Pada kasus 4 proses maupun 8 proses, terkadang *speedup* bisa lebih tinggi dari kasus 2 proses tetapi rata-rata *speedup*-nya lebih 
rendah serta efisiensinya jauh lebih rendah dari kasus 2 proses. Efisiensi menjadi lebih kecil karena pada kasus banyak proses, lebih 
banyak resource yang digunakan. Kesimpulannya program yang saya buat paling efektif dilakukan dengan 2 proses karena efisiensinya paling 
tinggi (mencapai 80~90%). 