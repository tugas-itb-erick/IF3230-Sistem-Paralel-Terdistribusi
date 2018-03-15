# OpenMP - Bitonic Sort
IF3230 - Sistem Paralel dan Terdistribusi
Erick Wijaya - 13515057

## Deskripsi Singkat
Program ini adalah ekstensi dari program bitonic sort yang disediakan oleh asisten. Program ini dapat mengeksekusi bitonic sort baik secara serial maupun secara paralel. Setelah sorting dilakukan, program akan mencetak waktu eksekusi rata-rata yang dibutuhkan untuk melakukan sort secara serial dan secara paralel. 

## Penggunaan Program
1. Lakukan kompilasi program dengan perintah ```make```.
2. Jalankan program dengan perintah ```./bitonic_sort n```, dengan ```n``` adalah ukuran array.
3. Hasil array sebelum dan sesudah di-sort dapat dilihat pada file ```data/input.txt``` dan ```data/output.txt```
4. Output waktu eksekusi serial dan paralel dapat dilihat pada file ```output/log.txt```

## Pembagian Tugas
Tugas ini dikerjakan oleh Erick Wijaya (13515057). 

## Laporan Pengujian
Pada bagian ini akan dijelaskan mengenai deskripsi solusi paralel yang digunakan, analisis solusi, jumlah thread, pengujian, dan analisis hasil uji. 

### Deskripsi Solusi Paralel

### Analisis Solusi


### Jumlah Thread
Jumlah thread yang digunakan adalah 4 karena PC saya memiliki 2 *physical core* dengan masing-masing core memiliki 2 *logical core*. Apabila saya menggunakan thread lebih dari itu, performansi program tidak melebihi program yang menggunakan  4 thread. Hal tersebut demikian karena PC saya memiliki total 4 core sehingga apabila menggunakan thread lebih dari 4, 

Selain itu, berdasarkan pengujian yang saya lakukan, penambahan thread lebih dari 4 tidak mempercepat waktu eksekusi program melebihi dengan menggunakan 4 thread. Hasil pengujian tersebut dapat dilihat pada folder **uji_thread**, nama file ```ujiX.txt``` dengan ```X``` mensimbolkan jumlah thread yang digunakan. 

### Pengukuran Kinerja (Tabel)
Berikut adalah tabel pengujian untuk bitonic sort serial dan paralel. (num_thread=4)

| *Ukuran Array* | *Percobaan 1 (mikrosekon)*    | *Percobaan 2 (mikrosekon)*    | *Percobaan 3 (mikrosekon)*    | *Percobaan 4 (mikrosekon)*    | Percobaan 5 (mikrosekon)    | *Rata-Rata (mikrosekon)*      |
| ------------ | --------------------------- | --------------------------- | --------------------------- | --------------------------- | --------------------------- | --------------------------- |
| 5000         | 4154.920578 (**serial**) 4968.881607 (**paralel**)     | 4054.069519 (**serial**) 5530.118942 (**paralel**)    | 4467.010498 (**serial**) 2573.966980 (**paralel**)    | 4848.957062 (**serial**) 10330.915451 (**paralel**)   |  3962.993622 (**serial**) 3566.980362 (**paralel**)   | 4297.590256 (**serial**) 5394.172668 (**paralel**)    |
| 50000        | 50214.052200 (**serial**) 37576.913834 (**paralel**)   | 56967.973709 (**serial**) 27436.971664 (**paralel**)   | 48700.094223 (**serial**) 26001.930237 (**paralel**)   | 51848.888397 (**serial**) 29010.057449 (**paralel**)   | 48896.074295 (**serial**) 30823.945999 (**paralel**)   | 51325.416565 (**serial**) 30169.963837 (**paralel**)   |
| 100000       | 111040.115356 (**serial**)   71974.039078 (**paralel**)  | 104829.072952 (**serial**)   59442.043304 (**paralel**)  | 104854.106903 (**serial**)   58248.996735 (**paralel**)  | 105112.075806 (**serial**)   54050.922394 (**paralel**)  | 108012.914658 (**serial**)   64358.949661 (**paralel**)  | 106769.657135 (**serial**) 61614.990234 (**paralel**)  |
| 200000       | 236206.054688 (**serial**) 136006.116867 (**paralel**) | 231781.959534 (**serial**) 122766.971588 (**paralel**) | 231312.990189 (**serial**) 121330.022812 (**paralel**) | 231589.078903 (**serial**) 121577.978134 (**paralel**) | 230040.073395 (**serial**) 122328.996658 (**paralel**) | 232186.031342 (**serial**) 124802.017212 (**paralel**) |
| 400000       | 514862.060547 (**serial**) 277946.949005 (**paralel**) | 522362.947464 (**serial**) 257061.958313 (**paralel**) | 520067.930222 (**serial**) 287576.913834 (**paralel**) | 510907.888412 (**serial**) 275991.916656 (**paralel**) | 509989.976883 (**serial**) 264043.092728 (**paralel**) | 515638.160706 (**serial**) 272524.166107 (**paralel**) |

### Analisis Kinerja Serial dan Paralel
Berdasarkan pengukuran kinerja pada poin sebelumnya, terlihat bahwa kinerja program paralel lebih cepat daripada serial, 
kecuali untuk kasus ukuran array 5000. Pada kasus array 5000, program serial lebih cepat karena program paralel membutuhkan waktu 
tambahan untuk melakukan context switching. Semakin besar jumlah thread, semakin banyak context switching yang terjadi. Akan tetapi, 
apabila menggunakan 2 thread, program paralel cenderung lebih cepat karena jumlah context switching lebih kecil. Namun, untuk program ini
saya tetap menggunakan 4 thread karena untuk kasus array yang 
berukuran lebih besar, bitonic sort dengan 4 thread bekerja lebih cepat ketimbang dengan 2 thread. 

Apabila dilihat dari tabel, efisiensi kinerja bitonic sort paralel dengan serial menjadi cenderung semakin tinggi, yaitu 
mulai dari .. hingga... Hal tersebut demikian karena waktu untuk context switching menjadi tidak signifikan apabila 
ukuran permasalahan (array) lebih besar. Kesimpulannya, bitonic sort paralel memiliki performa yang lebih baik daripada 
bitonic sort serial kecuali untuk kasus persoalan yang kecil. Semakin besar jumlah thread, semakin cepat performa program paralel 
tetapi dibutuhkan waktu context switching yang bisa menjadi signifikan untuk kasus persoalan yang kecil. Jumlah thread yang melebihi 
jumlah core tidak akan membuat program paralel menjadi semakin cepat dari program dengan jumlah thread sejumlah core. 
