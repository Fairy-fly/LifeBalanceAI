# coding: utf-8  
import codecs  
fp = r'D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp'  
f = codecs.open(fp, 'r', 'utf-8')  
lines = f.readlines()  
f.close()  
for i in range(0, 40): print(str(i+1)+':'+lines[i].rstrip())  
