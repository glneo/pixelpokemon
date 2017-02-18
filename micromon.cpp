#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

#include <SDL2/SDL.h>

using namespace std;

const int WIDTH = 512;
const int HEIGHT = 512;

class Micromon;

Micromon *microArray[HEIGHT][WIDTH];
vector<Micromon *> microList[2];
int attack[HEIGHT * WIDTH];
int defence[HEIGHT * WIDTH];

enum pokeType
{
	NORMAL = 0,
	FIGHTING,
	FLYING,
	POISON,
	GROUND,
	ROCK,
	BUG,
	GHOST,
	STEEL,
	FIRE,
	WATER,
	GRASS,
	ELECTRIC,
	PSYCHIC,
	ICE,
	DRAGON,
	DARK,
	FAIRY,
	MAXTYPE,
};

typedef uint32_t color;

#define COLOR(r, g, b) (color)((r & 0xff) <<  0 | \
                               (g & 0xff) <<  8 | \
                               (b & 0xff) << 16 | \
                               SDL_ALPHA_OPAQUE << 24)

color typeColor[] =
{
	COLOR(200, 200, 200), // NORMAL
	COLOR(153, 37, 33), // FIGHTING
	COLOR(154, 212, 237), // FLYING
	COLOR(100, 0, 200), // POISON
	COLOR(216, 170, 91), // GROUND
	COLOR(142, 110, 145), // ROCK
	COLOR(100, 255, 0), // BUG
	COLOR(41, 26, 79), // GHOST
	COLOR(100, 100, 100), // STEEL
	COLOR(255, 0, 0), // FIRE
	COLOR(0, 0, 255), // WATER
	COLOR(0, 255, 0), // GRASS
	COLOR(255, 255, 0), // ELECTRIC
	COLOR(255, 0, 255), // PSYCHIC
	COLOR(100, 100, 255), // ICE
	COLOR(98, 157, 209), // DRAGON
	COLOR(20, 20, 20), // DARK
	COLOR(255, 0, 100), // FAIRY
};

/* [Attacking Type][Defending type] */
double effectivenessTable[MAXTYPE][MAXTYPE] =
{
	1.0, 1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 0.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	2.0, 1.0, 0.5, 0.5, 1.0, 2.0, 0.5, 0.0, 2.0, 1.0, 1.0, 1.0, 1.0, 0.5, 2.0, 1.0, 2.0, 0.5,
	1.0, 2.0, 1.0, 1.0, 1.0, 0.5, 2.0, 1.0, 0.5, 1.0, 1.0, 2.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 1.0, 0.5, 0.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0,
	1.0, 1.0, 0.0, 2.0, 1.0, 2.0, 0.5, 1.0, 2.0, 2.0, 1.0, 0.5, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 0.5, 2.0, 1.0, 0.5, 1.0, 2.0, 1.0, 0.5, 2.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0,
	1.0, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 1.0, 2.0, 1.0, 2.0, 1.0, 1.0, 2.0, 0.5,
	0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 0.5, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 0.5, 0.5, 0.5, 1.0, 0.5, 1.0, 2.0, 1.0, 1.0, 2.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 0.5, 2.0, 1.0, 2.0, 0.5, 0.5, 2.0, 1.0, 1.0, 2.0, 0.5, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 1.0, 1.0, 1.0, 2.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0,
	1.0, 1.0, 0.5, 0.5, 2.0, 2.0, 0.5, 1.0, 0.5, 0.5, 2.0, 0.5, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0,
	1.0, 1.0, 2.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 0.5, 0.5, 1.0, 1.0, 0.5, 1.0, 1.0,
	1.0, 2.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0, 0.0, 1.0,
	1.0, 1.0, 2.0, 1.0, 2.0, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 2.0, 1.0, 1.0, 0.5, 2.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 0.0,
	1.0, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0, 1.0, 0.5, 0.5,
	1.0, 2.0, 1.0, 0.5, 1.0, 1.0, 1.0, 1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 1.0,
};

class Micromon
{
public:
	int id, x, y, type1, type2;
	int health, speed;
	int deadState;
	Micromon *neighbors[4];
	color myColor;

	Micromon(int y, int x) : x(x), y(y), deadState(0)
	{
		type1 = rand() % 18;
		if ((rand() % 4) == 0)
			type2 = rand() % 18;
		else
			type2 = -1;

		if (type2 == -1)
			myColor = typeColor[type1];
		else
		{
			myColor = COLOR(((2 * ((typeColor[type1] >> 0) & 0xff) / 3) + (((typeColor[type2] >> 0) & 0xff) / 3)),
				        ((2 * ((typeColor[type1] >> 8) & 0xff) / 3) + (((typeColor[type2] >> 8) & 0xff) / 3)),
				        ((2 * ((typeColor[type1] >> 16) & 0xff) / 3) + (((typeColor[type2] >> 16) & 0xff) / 3)));
		}

		int tot = 255;
		id = x + (y * WIDTH);
		health = 100 + (rand() % tot);
		tot -= (health - 100);
		attack[id] = (rand() % tot);
		tot -= attack[id];
		defence[id] = (rand() % tot);
		tot -= defence[id];
		speed = tot;
	}

	void linkNeighbors(Micromon *microArray[HEIGHT][WIDTH])
	{
		enum direction
		{
			north,
			south,
			east,
			west,
		};

		neighbors[north] = microArray[(y - 1) < 0 ? (HEIGHT - 1) : (y - 1)][x];
		neighbors[south] = microArray[(y + 1) >= HEIGHT ? 0 : (y + 1)][x];
		neighbors[east] = microArray[y][(x + 1) >= WIDTH ? 0 : (x + 1)];
		neighbors[west] = microArray[y][(x - 1) < 0 ? (WIDTH - 1) : (x - 1)];
	}

	int testdamage(Micromon *e)
	{
		int d = attack[this->id] - defence[e->id] / 2;

		if (d <= 0)
			return 1;

		d *= max(effectivenessTable[this->type1][e->type1], effectivenessTable[this->type2][e->type1]);

		return d;
	}

	Micromon *getWeakestNeighbor()
	{
		int highest_damage = -1;
		Micromon *highest = NULL;

		for (int i = 0; i < 4; i++)
			if (testdamage(this->neighbors[i]) > highest_damage)
			{
				highest_damage = testdamage(this->neighbors[i]);
				highest = this->neighbors[i];
			}

		return highest;
	}

	bool operator < (const Micromon &a) const
	{
		return speed < a.speed;
	}
};

void setup(Micromon *microArray[HEIGHT][WIDTH])
{
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
		{
			microArray[i][j] = new Micromon(i, j);
			microList[0].push_back(microArray[i][j]);
		}

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			microArray[i][j]->linkNeighbors(microArray);

	sort(begin(microList[0]), end(microList[0]));
}

void step()
{
	static int currMicro = 0;

	for (int currIndex = 0; currIndex < (HEIGHT * WIDTH); currIndex++)
	{
		Micromon *m = microList[currMicro][currIndex];
		if (m->deadState != currMicro)
			continue;

		m->deadState = (currMicro ^ 1);
		microList[(currMicro ^ 1)].push_back(m);

		Micromon *e = m->getWeakestNeighbor();
		if (e->id != m->id)
		{
			e->health -= m->testdamage(e);
			if (e->health <= 0)
			{
				m->health += 100;
				e->health = m->health;
				attack[m->id]++;
				attack[e->id]--;
				e->speed = m->speed;
				e->type1 = m->type1;
				e->type2 = m->type2;
				e->myColor = m->myColor;
				e->id = m->id;
				if (e->deadState == currMicro)
				{
					e->deadState = (currMicro ^ 1);
					microList[(currMicro ^ 1)].push_back(e);
				}
			}
		}
	}

	microList[currMicro].clear();
	currMicro ^= 1;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);

	SDL_Window *window = SDL_CreateWindow("PixelMon",
	                                      SDL_WINDOWPOS_UNDEFINED,
	                                      SDL_WINDOWPOS_UNDEFINED,
	                                      WIDTH, HEIGHT,
	                                      SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_Log("Unable to create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer,
	                                         SDL_PIXELFORMAT_ABGR8888,
	                                         SDL_TEXTUREACCESS_STREAMING,
	                                         WIDTH, HEIGHT);
	if (texture == NULL)
	{
		SDL_Log("Unable to create texture: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	setup(microArray);

	uint32_t (*pixels)[HEIGHT] = new uint32_t[HEIGHT][WIDTH];

	SDL_Event event;
	bool running = true;
	while (running)
	{
		const Uint64 start = SDL_GetPerformanceCounter();

		while (SDL_PollEvent(&event))
		{
			if ((event.type == SDL_QUIT) ||
				(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
			{
				running = false;
				break;
			}
		}

		step();

		for (int i = 0; i < HEIGHT; i++)
			for (int j = 0; j < WIDTH; j++)
				pixels[i][j] = microArray[i][j]->myColor;

		SDL_UpdateTexture(texture, NULL, pixels, sizeof(*pixels));

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		const Uint64 end = SDL_GetPerformanceCounter();
		const static Uint64 freq = SDL_GetPerformanceFrequency();
		const double seconds = (end - start) / static_cast<double>(freq);
		cout << "Frame time: " << seconds * 1000.0 << "ms" << endl;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
