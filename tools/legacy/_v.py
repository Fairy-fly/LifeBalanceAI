# coding: utf-8  
import codecs  
fp = r'D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp'  
f = codecs.open(fp, 'r', 'utf-8')  
lines = f.readlines()  
f.close()  
for i in range(82, 112): print(str(i+1)+':'+lines[i].rstrip())  
print('---')  
for i in range(len(lines)):  
    if 'setCurrentIndex(2)' in lines[i]:  
        for j in range(i-1, i+3): print(str(j+1)+':'+lines[j].rstrip())  
        print('---')  
