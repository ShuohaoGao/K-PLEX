g++ -std=c++11 -O3 -w MDP-bb.cpp -o MDP-bb -DNDEBUG -DNO_PROGRESS_BAR # -DVERIFY

date >> log/run_log.txt


echo "============== Amazon0601 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Amazon0601_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== as-caida20040105 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/as-caida20040105_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== dblp-2010 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/dblp-2010_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== email-eu-all ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-eu-all_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== email-Eu-core ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== enron ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enron_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== enwiki-2022 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/enwiki-2022_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== hollywood-2011 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/hollywood-2011_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== imdb-2021 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/imdb-2021_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== it-2004 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/it-2004_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== p2p-Gnutella06 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/p2p-Gnutella06_K=5_L=5.txt 5 5 >> log/run_log.txt




echo "============== Slashdot0811 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Slashdot0811_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== soc-Epinions1 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-Epinions1_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== soc-LiveJournal1 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-LiveJournal1_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== soc-pokec-relationships ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-pokec-relationships_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== soc-sign-Slashdot090221 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/soc-sign-Slashdot090221_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== sx-stackoverflow ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/sx-stackoverflow_K=5_L=5.txt 5 5 >> log/run_log.txt


echo "============== uk-2002 ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/uk-2002_K=5_L=5.txt 5 5 >> log/run_log.txt




echo "============== web-Google ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/web-Google_K=5_L=5.txt 5 5 >> log/run_log.txt




echo "============== WikiTalk ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/WikiTalk_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 186400s ./MDP-bb ../reduced_graph/WikiTalk_K=5_L=5.txt 5 5 >> log/run_log.txt




echo "============== Wiki-Vote ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/Wiki-Vote_K=5_L=5.txt 5 5 >> log/run_log.txt

date >> log/run_log.txt
echo "ok" >> log/run_log.txt
echo "" >> log/run_log.txt