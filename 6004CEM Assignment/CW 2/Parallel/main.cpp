#include <iostream>
#include <cmath>

#define PART_CYCLE_STEPS 50
#define sqr(a) (a)*(a)

/* PART A
 * You can change OpenMP schedule type here
*/
#define SCHEDULE schedule(dynamic, 2)


struct Particle
{
    int x;
    int y;
    int z;
};

static Particle heap[] = {
    {   5,   4,  10 }
  , {   7,  -8,  14 }
  , {  -2,   9,   8 }
  , {  15,  -6,   3 }
  , {  12,   4,  -5 }
  , {   4,  20,  17 }
  , { -16,   5,   1 }
  , { -11,   3,  16 }
  , {   3,  10, -19 }
  , { -16,   7,   4 }
};


/* Calculate distance between points.
 *
 * param[in]  a point.
 * param[in]  b point.
 * return distance between points.
*/
static inline unsigned int distance(Particle const a, Particle const b)
{
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y) + sqr(a.z - b.z));
}

/* Print all points.
 *
 * param[in]  original massive.
 * param[in]  size of massive.
 * param[in]  distance print flag.
 * param[in]  central point to calculate distance.
*/
static void printHeap(
    Particle *const arr
  , size_t const size
  , bool const is_dist = false
  , Particle const center = { 0 }
){
    for (int i = 0; i < size; i++)
    {
        std::cout << i + 1 << ": (" << arr[i].x;
        std::cout << ", " << arr[i].y;
        std::cout << ", " << arr[i].z;
        std::cout << ")";

        if (is_dist)
        {
            std::cout << " dist: " << distance(center, arr[i]);
        }
        std::cout << std::endl;
    }
}

/* Calculate center of heap.
 *
 * param[in]  original massive.
 * param[in]  size of massive.
 * return distance between points.
*/
static Particle locateCenter(Particle *const arr, size_t const size)
{
    Particle center = { 0 };

    #pragma omp parallel if (size > 4)
    {
        #pragma omp for SCHEDULE
        for (int i = 0; i < size; i++)
        {
            center.x += arr[i].x;
            center.y += arr[i].y;
            center.z += arr[i].z;
        }
    }
    center.x /= (int)size;
    center.y /= (int)size;
    center.z /= (int)size;

    return center;
}


int main(void)
{
    unsigned int alt = 2; // particle position alteration variable
    size_t particle_count = sizeof(heap) / sizeof(struct Particle);

    /*
     * PART B
    */
    std::cout << "---zero step---" << std::endl;
    printHeap(heap, particle_count);
    #pragma omp parallel if (particle_count > 4) shared(alt, heap)
    {
        for (int j = 0; j < PART_CYCLE_STEPS; j++)
        {
            #pragma omp for SCHEDULE
            for (int i = 0; i < particle_count; i++)
            {
                switch (rand() % 3)
                {
                    case 0:
                        heap[i].x += alt * (rand() % 2 ? 1 : -1);
                        heap[i].y += alt * (rand() % 2 ? 1 : -1);
                        break;
                    case 1:
                        heap[i].y += alt * (rand() % 2 ? 1 : -1);
                        heap[i].z += alt * (rand() % 2 ? 1 : -1);
                        break;
                    case 2:
                        heap[i].x += alt * (rand() % 2 ? 1 : -1);
                        heap[i].z += alt * (rand() % 2 ? 1 : -1);
                        break;
                }
            }
            #pragma omp single
            {
                alt += rand() % 5 + 1;

                if (j == PART_CYCLE_STEPS / 2)
                {
                    std::cout << "\n---" << PART_CYCLE_STEPS / 2 << " step---\n";
                    printHeap(heap, particle_count);
                }
            }
        }
    }
    std::cout << "\n---" << PART_CYCLE_STEPS << " step---\n";
    printHeap(heap, particle_count);

    /*
     * PART C
    */
    alt = 2;
    Particle center = locateCenter(heap, particle_count);

    std::cout << std::endl << "Center of Heap:";
    std::cout << " (" << center.x;
    std::cout << ", " << center.y;
    std::cout << ", " << center.z << ")" << std::endl;

    std::cout << "\n---Original distances---\n";
    printHeap(heap, particle_count, true, center);

    #pragma omp parallel if (particle_count > 4) shared(alt, heap)
    {
        for (int j = 0; j < PART_CYCLE_STEPS; j++)
        {
            #pragma omp for SCHEDULE
            for (int i = 0; i < particle_count; i++)
            {
                switch (rand() % 3)
                {
                    case 0:
                        heap[i].x += alt * (heap[i].x < center.x ? 1 : -1);
                        heap[i].y += alt * (heap[i].y < center.y ? 1 : -1);
                        break;
                    case 1:
                        heap[i].y += alt * (heap[i].y < center.y ? 1 : -1);
                        heap[i].z += alt * (heap[i].z < center.z ? 1 : -1);
                        break;
                    case 2:
                        heap[i].x += alt * (heap[i].x < center.x ? 1 : -1);
                        heap[i].z += alt * (heap[i].z < center.z ? 1 : -1);
                        break;
                }
            }
            #pragma omp single
            {
                alt += rand() % 5 + 1;

                if (j == PART_CYCLE_STEPS / 2)
                {
                    std::cout << "\n---" << PART_CYCLE_STEPS / 2 << " step---\n";
                    printHeap(heap, particle_count, true, center);
                }
            }
        }
    }

    std::cout << "\n---" << PART_CYCLE_STEPS << " step---\n";
    printHeap(heap, particle_count, true, center);

    return 0;
}

