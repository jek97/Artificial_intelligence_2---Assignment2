import random as r
import math as m

# open the files 
wp_file = open("waypoint.txt", "w")
N = 6 # number of nodes connected to the actual one
# note the region file will always be the same, so we don't need to create it all the times

# initialize the array with the regions points
wp_arr = [[0,0,0], [-2,2,0], [2,2,0], [-2,-2,0], [2,-2,0], [3,0,0]]
dist = []
conn = []
#wp_file.write("Now the file has more content!")
#wp_file.close()

# create the waypoints
r.seed(10)
for i in range(24):
    x = r.uniform(-2, 2, 1)
    y = r.uniform(-2, 2, 1)
    wp = [x, y, 0]
    wp_arr.append(wp)

# associate the connected waypoint as the N closer waypoints
for i in range(len(wp_arr)):
    for j in range(len(wp_arr)):
        dist[j] = m.sqrt(m.pow((wp_arr[i][0] - wp_arr[j][0]), 2) + m.pow((wp_arr[i][1] - wp_arr[j][1]), 2) # evaluate the distances vector
    
    for k in range(N):
        dist_k = min(dist)
        indx = dist.index(min(dist))
        dist[indx] = 1000
        conn[i].append([indx, dist_k])
        conn[indx].append([i, dist_k])



    

            
            

