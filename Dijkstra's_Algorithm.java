import java.util.*;

public class DijkstraAlgorithm {
    public static void dijkstra(Map<Integer, List<int[]>> graph, int start) {
        PriorityQueue<int[]> pq = new PriorityQueue<>(Comparator.comparingInt(a -> a[1]));
        pq.add(new int[]{start, 0});

        Map<Integer, Integer> distance = new HashMap<>();
        distance.put(start, 0);

        while (!pq.isEmpty()) {
            int[] node = pq.poll();
            int current = node[0];
            int dist = node[1];

            if (distance.get(current) < dist) {
                continue;
            }

            for (int[] neighbor : graph.get(current)) {
                int next = neighbor[0];
                int cost = neighbor[1] + dist;

                if (!distance.containsKey(next) || cost < distance.get(next)) {
                    distance.put(next, cost);
                    pq.add(new int[]{next, cost});
                }
            }
        }

        System.out.println("Shortest Distances: " + distance);
    }

    public static void main(String[] args) {
        Map<Integer, List<int[]>> graph = new HashMap<>();
        graph.put(0, Arrays.asList(new int[]{1, 4}, new int[]{2, 1}));
        graph.put(1, Arrays.asList(new int[]{3, 5}));
        graph.put(2, Arrays.asList(new int[]{1, 2}, new int[]{3, 2}));
        graph.put(3, Arrays.asList());

        dijkstra(graph, 0);
    }
}
