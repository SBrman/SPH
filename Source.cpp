#include "Vec.hpp"
#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <array>
#include <list>

#include <cmath>
#include <iostream>


const int WIDTH = 1000;
const int SCREEN_DIVISIONS = 40;

const float K_SMOOTH_RADIUS = 35;
const float K_STIFF = 150.0;
const float K_STIFF_N = 1000.0;
const float K_REST_DENSITY = 0.002;
const float GRAB_RADIUS = 0.08 * WIDTH;

const int ROW = 30;
const int PARTICLE_PER_ROW = 30;
const int NUM_PARTICLES = ROW * PARTICLE_PER_ROW;
const int RADIUS = 9;

const int GRID_N = WIDTH / SCREEN_DIVISIONS;

const float MAX_PRESSURE = 30;
const float MAX_NEAR_PRESSURE = 300;

const int SPACING = 30;

struct Particle {
    Vec pos, oldPos, vel;
    float press, dens;
    float pressN, densN;
    bool grabbed;

	Particle() {
		Particle(Vec{ 0.0, 0.0 });
	};

	Particle(Vec pn) : pos(pn), oldPos(pn), press(0), dens(0), pressN(0), densN(0), grabbed(false) {
		vel = Vec{ 1000.0, 0.0 };
	};
};




struct Pair {
	Particle& p1, & p2;      // particles in the pair
	float q, q2, q3;		 // q is (1-d/h), q2 is q^2, q3 is q^3
	Pair(Particle& pp1, Particle& pp2, float qq) :
		p1(pp1), p2(pp2), q(qq), 
		q2(qq * qq), q3(qq * qq * qq) {};
};



// Divides the screen into 10x10 squares and returns the index of the square that the particle is in
int screen_index(const Particle& particle){
	int x = static_cast<int>(std::floor(particle.pos.x / SCREEN_DIVISIONS));
	int y = static_cast<int>(std::floor(particle.pos.y / SCREEN_DIVISIONS));
	
	int hash = x * 89 + y * 97;
	return hash % (2 * WIDTH / SCREEN_DIVISIONS);
}


void collision_response(Particle& p)
{
	if (p.pos.y < RADIUS) {
		p.pos.y = RADIUS;
		p.vel.y *= -0.3f;
	}
	if (p.pos.y > WIDTH) {
		p.pos.y = WIDTH - RADIUS;
		p.vel.y *= -0.3f;
	}
	if (p.pos.x < 0) {
		p.pos.x = RADIUS;
		p.vel.x *= -0.3f;
	}
	if (p.pos.x > WIDTH) {
		p.pos.x = WIDTH - RADIUS;
		p.vel.x *= -0.3f;
	}
}



void simulateSPH( std::array<Particle, NUM_PARTICLES>& particles, Vec mousePos, float dt)
{

	//Integrate velocity & gravity to update positions
	for (auto& p : particles) {
		p.vel = (p.pos - p.oldPos) / dt;   //Compute vel from positions updated via pressure forces
		p.vel.x = clamp(p.vel.x, -500.0, 500.0);
		p.vel.y = clamp(p.vel.y, -100.0, 100.0);
		p.vel += Vec(0, 500) * dt;          //Integrate velocity based on gravity

		//Collision response
		collision_response(p);

		// if (p.grabbed) p.vel += ((mousePos - p.pos) / GRAB_RADIUS - p.vel) * 100 * dt; //Move grabbed particles towards mouse

		p.oldPos = p.pos;
		p.pos += p.vel * dt;              //Integrate position based on velocity
		
		// p.pos.x = clamp(p.pos.x, 0.000001, WIDTH-0.00000001);
		// p.pos.y = clamp(p.pos.y, 0.000001, WIDTH-0.00000001);
		p.dens = 0.0f;
		p.densN = 0.0f;


	}
	
	std::array<std::vector<Particle*>, SCREEN_DIVISIONS*SCREEN_DIVISIONS> cc;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		for (int j = 0; j < SPACING+1; j += SPACING) {
			int col = clamp(std::floor((particles[i].pos.x + j) / SCREEN_DIVISIONS), 0, SCREEN_DIVISIONS-1);
			int row = clamp(std::floor((particles[i].pos.y + j) / SCREEN_DIVISIONS), 0, SCREEN_DIVISIONS-1);

			cc[(int)(col + SCREEN_DIVISIONS * row)].push_back(&particles[i]);
		}
	}

	//Find all neighboring particles [TODO: Slow for large SPH systems!]
	std::vector<Pair> pairs;
	// for (int i = 0; i < NUM_PARTICLES; i++) {
	// 	for (int j = i + 1; j < NUM_PARTICLES; j++) {
	// 		float dist = particles[i].pos.distanceTo(particles[j].pos);
	// 		// if (dist < k_smooth_radius && i < j) {
	// 		if (dist < K_SMOOTH_RADIUS) {
	// 			float q = 1 - (dist / K_SMOOTH_RADIUS);
	// 			pairs.push_back(Pair(particles[i], particles[j], q));
	// 		}
	// 	}
	// }

	for (int s = 0; s < SCREEN_DIVISIONS*SCREEN_DIVISIONS; s++) {
		for (int i = 0; cc[s].size() > 1 && i < cc[s].size() - 1; i++) {
			for (int j = i + 1; j < cc[s].size(); j++) {
				float dist = cc[s][i]->pos.distanceTo(cc[s][j]->pos);
				// if (dist < k_smooth_radius && i < j) {
				if (dist < K_SMOOTH_RADIUS) {
					float q = 1 - (dist / K_SMOOTH_RADIUS);
					pairs.push_back(Pair(*cc[s][i], *cc[s][j], q));
				}
			}
		}
	}
	
	//Accumulate per-particle density
	for (Pair& p : pairs) {
		p.p1.dens += p.q * p.q;
		p.p2.dens += p.q * p.q;
		p.p1.densN += p.q * p.q * p.q;
		p.p2.densN += p.q * p.q * p.q;
	}

	//Computer per-particle pressure: stiffness*(density - rest_density)
	for (auto& p : particles) {
		p.press = K_STIFF * (p.dens - K_REST_DENSITY);
		p.pressN = K_STIFF_N * (p.densN);
		
		p.press = std::max(p.press, MAX_PRESSURE);
		p.pressN = std::max(p.pressN, MAX_NEAR_PRESSURE);
	}

	for (Pair& pair : pairs) {
		Particle& a = pair.p1;
		Particle& b = pair.p2;
		float total_pressure = (a.press + b.press) * pair.q + (a.pressN + b.pressN) * pair.q2;
		float displace = total_pressure * (dt * dt);
		a.pos += (a.pos - b.pos).normalized() * displace;
		b.pos += (b.pos - a.pos).normalized() * displace;
	}
}



int main() {

	std::array<Particle, NUM_PARTICLES> particles;
	int c = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < PARTICLE_PER_ROW; j++) {
			// float x = -1 + 2 * j / n + 0.1 * i / r;
			// float y = ASPECT_RATIO * (0.5 - 1.5 * i / r);
			float x = i * 30;
			float y = j * 30 + 500;
			particles[c] = Particle(Vec{ x, y });
			c++;
		}
	}

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WIDTH, WIDTH, "SPH Simulation");
	SetTargetFPS(60);

	float last_frame_start = 0;
	while (!WindowShouldClose()) {
		float dt = 1/60.0f; //ms to s

		//Note which particles to grab/relase based on mouse
		auto mouse = GetMousePosition();
		Vec mousePos = Vec(mouse.x, mouse.y);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			for (auto& p : particles) {
				if (mousePos.distanceTo(p.pos) < GRAB_RADIUS) {
					p.grabbed = true;
				}
			}
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			for (auto& p : particles) {
				p.grabbed = false;
			}
		}

		//Substep simulation for increased numerical stability
		int ss = 30;
		for (int i = 0; i < ss; i++) {
			float sim_dt = dt / (float) (ss);
			if (sim_dt > 0.003f) 
				sim_dt = 0.003f; //enforce maximum timestep
			simulateSPH(particles, mousePos, sim_dt);
		}

		//Draw the background & SPH particles
		BeginDrawing();
		ClearBackground(BLACK);
		int i = 0;
		for (auto& p : particles) {
			float q = (p.press) / 30.0f;                     //normalize pressure to 1
			// Color particleColor = { (unsigned char)(255 * (0.7f - q * 0.5f)), (unsigned char)(255 * (0.8f - q * 0.4f)), (unsigned char)(255 * (1.0f - q * 0.2f)), 255 }; //color particles by pressure (white->blue)
			Color particleColor = {(unsigned char)(255 * (1.0f - q * 0.2f)), 255, 255, 255 }; //color particles by pressure (white->blue)
			
			if (i % 101 == 0) {
				DrawCircle(p.pos.x, p.pos.y, 2*RADIUS, BLUE);    //x,y,radius
			}
			else
				DrawCircle(p.pos.x, p.pos.y, RADIUS, particleColor);    //x,y,radius
			// DrawRectangle(p.pos.x, p.pos.y, 6, 6, particleColor);
			// DrawCircle(p.pos.x, p.pos.y, 6, WHITE);    //x,y,radius
			i++;
		}
		EndDrawing();
	}

	CloseWindow();
}
