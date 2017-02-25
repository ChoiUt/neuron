


import pandas as pd
from sklearn.decomposition import PCA
from sklearn import svm
from sklearn import cross_validation
import time
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter
from mlxtend.plotting import plot_decision_regions


data = pd.read_csv('allplus3R.csv', header=0)
tdata1 = pd.read_csv('test5.csv', header=0)

X= data.values[0:, 1:]  
y= data.values[0:,0]
h = .02
y = list(map(int, y))
y=np.asarray(y)

pca = PCA(n_components = 2,whiten = True)
train_x = pca.fit_transform(X)

clf = svm.SVC(kernel = 'rbf',C = 1000)
clf.fit(train_x,y)

# a mesh to plot in
x_min, x_max = train_x[:, 0].min() - 1, train_x[:, 0].max() + 1
y_min, y_max = train_x[:, 1].min() - 1, train_x[:, 1].max() + 1

xx, yy = np.meshgrid(np.arange(x_min, x_max, h),np.arange(y_min, y_max, h))
Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])
# Put the result into a color plot
Z = Z.reshape(xx.shape)
plt.contourf(xx, yy, Z, cmap=plt.cm.coolwarm)

# Plot also the training points
plt.scatter(train_x[:, 0], train_x[:, 1], c=y, cmap=plt.cm.coolwarm)
plt.xlabel('Sepal width')
plt.ylabel('Sepal length')
plt.xlim(xx.min(), xx.max())
plt.ylim(yy.min(), yy.max())
plt.legend(loc='upper left')
plt.xticks(())
plt.yticks(())
plt.title('SVC with RBF kernel')

plt.show()
