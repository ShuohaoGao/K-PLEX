g++ -std=c++11 -O3 -w main.cpp -o kpbb -DNDEBUG -DNO_PROGRESS_BAR # -DVERIFY -DNO_SQRT

date >> log/kpbb-log.txt

# bin graph file
list=(
"brock200-2"
)
for item in "${list[@]}"  
do  
    for k in $(seq 2 5); do
        echo "============== "$item"  "$k" =============="  >> log/kpbb-log.txt
        timeout 3600s ./kpbb ../data/bin/"$item".bin $k >> log/kpbb-log.txt
        echo "-------------- "$item"  "$k" --------------"  >> log/kpbb-log.txt
        echo ""  >> log/kpbb-log.txt
    done
done

# edges graph file
list=(
"email-Eu-core"
"WikiTalk"
)
for item in "${list[@]}"  
do  
    for k in $(seq 2 5); do
        echo "============== "$item"  "$k" =============="  >> log/kpbb-log.txt
        timeout 3600s ./kpbb ../data/edges/"$item".out $k >> log/kpbb-log.txt
        echo "-------------- "$item"  "$k" --------------"  >> log/kpbb-log.txt
        echo ""  >> log/kpbb-log.txt
    done
done

date >> log/kpbb-log.txt
echo "ok" >> log/kpbb-log.txt
echo "" >> log/kpbb-log.txt