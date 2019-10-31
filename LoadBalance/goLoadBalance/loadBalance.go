package main

import(
	"fmt"
	"sync"
	"strconv"
	"hash/fnv"
	"crypto/sha1"
	"encoding/binary"
)
const MAX_NUM = 100
type loadBalancer struct{
	
	registedIP map[int]*IP
	wmutex sync.RWMutex
	roundRobinCur int 
	weights []int
}
type IP struct{
	ip string
	weight int 
	ping  float32
}
func main(){
	loadBalancer := &loadBalancer{}
	loadBalancer.registedIP = make(map[int]*IP)
	loadBalancer.roundRobinCur = 0;
	loadBalancer.weights = make([]int,10,20)
	for i:=0; i<10; i++{
		loadBalancer.registerIp(&IP{"127.0.0."+strconv.Itoa(i),i,0.0})
	}
	// for i:=0; i<10; i++{
	// 	fmt.Println((*loadBalancer.registedIP[i]).ip)
	// }
	fmt.Println("-----roundRobin------")
	for i:=0; i<20; i++{
		fmt.Println(loadBalancer.roundRobin())
		fmt.Println(loadBalancer.weights[i])
	}
	fmt.Println("------------")
	for i:=0;i<5;i++{
		fmt.Println("hash:")
		fmt.Println("selected:"+loadBalancer.hash("192.168.70."+strconv.Itoa(i)))
		fmt.Println("ihash:")
		fmt.Println("selected:"+loadBalancer.ihash("192.168.70."+strconv.Itoa(i)))
	}

}
func (ld *loadBalancer) roundRobin() string {
	ld.wmutex.RLock()
	defer ld.wmutex.RUnlock()
	// 上锁，other 可读不可写
	if ld.roundRobinCur>=len(ld.registedIP){
		ld.roundRobinCur = 0;
	}
	ld.roundRobinCur++
	return (*ld.registedIP[ld.roundRobinCur-1]).ip
}
func(ld *loadBalancer) registerIp(ip *IP){
	ld.wmutex.RLock()
	defer ld.wmutex.RUnlock()
	ld.weights = append(ld.weights,ip.weight+ld.weights[len(ld.weights)-1])
	ld.registedIP[len(ld.registedIP)] = ip
}
func(ld *loadBalancer) hash(remoteIp string) string{
	hash := sha1.New()
	//创建 hash
	hash.Write([]byte(remoteIp))
	bs := hash.Sum(nil)
	R := len(ld.registedIP)
	//大字节序
	hashInt := binary.BigEndian.Uint64(bs)
	res := int(hashInt % uint64(R))
	return (*ld.registedIP[res]).ip
}
func(ld *loadBalancer) ihash(remoteIp string) string{
	hash := fnv.New32a()
	hash.Write([]byte(remoteIp))
	return(*ld.registedIP[int(hash.Sum32() & 0x7fffffff)%len(ld.registedIP)]).ip

}