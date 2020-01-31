#to understand how it works, think that it is on the epos folder... like trunk/auto_batch_comp.sh
echo Remember to enable the Monitor and previously set the Monitor frequencies at $1 traits!!
((counter = 0+0))
((old = 0+0))
while [ $counter -lt 52 ]
do
	((temp = $counter*4+3)) #makes all the sequential tests
	echo temp = $temp
	sed -i 's/static const unsigned int PMU_EVENT3        = '$old';/static const unsigned int PMU_EVENT3        = '$temp';/' ./app/$1/$1_traits.h
	((old = $old+1))
	((temp = $temp+1)) 
	echo temp = $temp
	sed -i 's/static const unsigned int PMU_EVENT4        = '$old';/static const unsigned int PMU_EVENT4        = '$temp';/' ./app/$1/$1_traits.h
	((old = $old+1))
	((temp = $temp+1))
	echo temp = $temp
	sed -i 's/static const unsigned int PMU_EVENT5        = '$old';/static const unsigned int PMU_EVENT5        = '$temp';/' ./app/$1/$1_traits.h
	((old = $old+1))
	((temp = $temp+1)) #ends with
	sed -i 's/static const unsigned int PMU_EVENT6        = '$old';/static const unsigned int PMU_EVENT6        = '$temp';/' ./app/$1/$1_traits.h
	echo temp = $temp
	make veryclean >> ./../out/clean$counter.txt && make APPLICATION=$1 >> ./../out/comp$counter.txt
	if [ $counter -lt 10 ]
	then
		cp ./img/$1.img ./../imgs/epos0$counter.img
	else
		cp ./img/$1.img ./../imgs/epos$counter.img
	fi
	((counter = $counter + 1))
	((old = $temp-3))
	echo "END" $counter
done
((temp = 211)) #make last test
echo temp = $temp
sed -i 's/static const unsigned int PMU_EVENT3        = '$old';/static const unsigned int PMU_EVENT3        = '$temp';/' ./../../app/$1/$1_traits.h
((old = $old+1))
((temp = $temp+1)) #212
echo temp = $temp
sed -i 's/static const unsigned int PMU_EVENT4        = '$old';/static const unsigned int PMU_EVENT4        = '$temp';/' ./../../app/$1/$1_traits.h
((old = $old+1))
((temp = $temp+1)) #213
echo temp = $temp
sed -i 's/static const unsigned int PMU_EVENT5        = '$old';/static const unsigned int PMU_EVENT5        = '$temp';/' ./../../app/$1/$1_traits.h
#210 will be repeated
echo temp = $temp
make veryclean >> ./../out/clean$counter.txt && make APPLICATION=$1 >> ./../out/comp$counter.txt
if [ $counter -lt 10 ]
then
    cp ./img/$1.img ./../imgs/epos0$counter.img
else
    cp ./img/$1.img ./../imgs/epos$counter.img
fi
((counter = $counter + 1))
((old = $temp-3))
echo "END" $counter
scp ./../imgs/*.img entropy_proxy:/var/ftpd/imgs/
