import pandas as pd
from sklearn.decomposition import PCA
from sklearn import svm
from sklearn import cross_validation
import time
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter
from mlxtend.plotting import plot_decision_regions


data = pd.read_csv('allplus3R_moon.csv', header=0)
tdata1 = pd.read_csv('test2_moon.csv', header=0)

X= data.values[0:, 1:]  
y= data.values[0:,0]
h = .02
y = list(map(int, y))
y=np.asarray(y)

pca = PCA(n_components = 2,whiten = True)
train_x = pca.fit_transform(X)

clf = svm.SVC(kernel = 'rbf',C = 1000)
clf.fit(train_x,y)
x_min, x_max = train_x[:, 0].min() - 1, train_x[:, 0].max() + 1
y_min, y_max = train_x[:, 1].min() - 1, train_x[:, 1].max() + 1
xx, yy = np.meshgrid(np.arange(x_min, x_max, h),np.arange(y_min, y_max, h))
Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])
Z = Z.reshape(xx.shape)
plt.contourf(xx, yy, Z, cmap=plt.cm.coolwarm)

idx_1 = np.where(y == 1)
p1 = plt.scatter(train_x[idx_1,0], train_x[idx_1,1], marker = '1', color = 'm', label='1')  
idx_2 = np.where(y == 2)
p2 = plt.scatter(train_x[idx_2,0], train_x[idx_2,1], marker = '2', color = 'c', label='2')  
idx_3 = np.where(y == 3) 
p3 = plt.scatter(train_x[idx_3,0], train_x[idx_3,1], marker = '3', color = 'r', label='3')  
idx_4 = np.where(y == 4) 
p4 = plt.scatter(train_x[idx_4,0], train_x[idx_4,1], marker = '4', color = 'g', label='4') 
idx_5 = np.where(y == 5) 
p5 = plt.scatter(train_x[idx_5,0], train_x[idx_5,1], marker = 'X', color = 'y', label='5')  

plt.xlabel('width')
plt.ylabel('height')
plt.xlim(x_min, x_max)
plt.ylim(y_min, y_max)
plt.legend(loc='upper left')
plt.xticks(())
plt.yticks(())
plt.title('SVC with RBF kernel')
plt.show()
