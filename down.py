
import pandas as pd
from sklearn.decomposition import PCA
from sklearn import svm
from sklearn import cross_validation
import time
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter

def shut_down(s):
	if s ==1.0:
		return "你伸出了一隻手指"
	elif s ==2.0:
		return "你伸出了二隻手指"
	elif s ==3.0:
		return "你伸出了三隻手指"
	elif s ==4.0:
		return "你伸出了四隻手指"
	elif s ==5.0:
		return "你伸出了五隻手指"
	else:
		return "對不起，分析失敗"
		
		
train_num = 13000
test_num = 16000
data = pd.read_csv('allplus3R.csv', header=0)
tdata1 = pd.read_csv('test5.csv', header=0)

X= data.values[0:, 1:]  
y= data.values[0:,0]
h = .02

X_train, X_test, y_train, y_test = cross_validation.train_test_split(X, y, test_size=0.4, random_state=0)
train_data = data.values[0:train_num,1:]
train_label = data.values[0:train_num,0]
test_data = data.values[train_num:test_num,1:]
test_label = data.values[train_num:test_num,0]

tdata=tdata1.values[0:,1:]
t = time.time()
pca = PCA(n_components = 10,whiten = True)
train_x = pca.fit_transform(train_data)
test_x = pca.transform(test_data)

test_td = pca.transform(tdata)

#svm方法

clf = svm.SVC(kernel = 'rbf',C = 10)
clf.fit(train_x,train_label)

print("cross_validation score:")
print(u'%f \n' %clf.score(train_x,train_label))

pre = clf.predict(test_x)
pre2 = clf.predict(test_td)
acc = float((pre==test_label).sum())/len(test_x)

# print ("csv內每一維的訊息：")
# print (data.info())
print ("經過PCA後train_x第0 INDEX 的特徵數量：")
print (len(train_x[0]))
print ("經過PCA後train_x第0 INDEX 的數據：")
print (train_x[0])
print ("經過PCA後train_x的數據：")
print (len(train_x))
print ("經過PCA後test_x的數據：")
print (len(test_x))
print (u'accuracy：%f,waist time：%.2fs \n' %(acc,time.time()-t))
print ("伸出的手指：")
print (shut_down(Counter(pre2).most_common(1)[0][0]) )