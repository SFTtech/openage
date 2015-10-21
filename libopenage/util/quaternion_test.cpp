// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "quaternion.h"

#include <cmath>

#include "../log/log.h"
#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

void quaternion() {
	{
		// Quaternion construction tests
		const Quaternion<> id{};
		Quaternion<> id_explicit{1.0, 0.0, 0.0, 0.0};

		id.equals(id_explicit) or TESTFAIL;
		id.equals_deg(id_explicit, 1e-5f) or TESTFAIL;

		Quaternion<> wrong{0.0, 0.0, 1.0, 0.0};
		id.equals(wrong) and TESTFAIL;

		Matrix3 id_mat3 = Matrix3::identity();
		Quaternion<> q_id_mat3{id_mat3};
		id.equals(q_id_mat3) or TESTFAIL;

		Matrix4 id_mat4 = Matrix4::identity();
		Quaternion<> q_id_mat4{id_mat4};
		id.equals(q_id_mat4) or TESTFAIL;

		Matrix4 neg_mat4{
			-1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1,
		};

		Quaternion<> q_neg{neg_mat4};
		Quaternion<> q_nex_exp{0, 0, 1, 0};

		q_neg.equals(q_nex_exp) or TESTFAIL;
	}
	{
		// member functions
		Quaternion<> q0{1, 2, 3, 4};
		Quaternion<> q1{5, 6, 7, 8};

		TESTEQUALS_FLOAT(q0.dot(q1), 5 + 12 + 21 + 32, 1e-5f);
		TESTEQUALS_FLOAT(q0.norm(), 5.4772255 /*= sqrt(30) */, 1e-4f);

		Quaternion<> q2{2, 8, 4, 16};

		Quaternion<> q2_normd = q2.normalized();
		q2_normd.equals(q2) or TESTFAIL;

		Quaternion<> q2conj_exp{2, -8, -4, -16};
		q2conj_exp.equals(q2.conjugated()) or TESTFAIL;

		Quaternion<> q2inv_exp{
			0.0058823529411764705,
			-0.023529411764705882,
			-0.011764705882352941,
			-0.047058823529411764
		};

		Quaternion<> q2inv = q2.inversed();
		q2inv.normalized().equals(q2inv_exp.normalized()) or TESTFAIL;

		q2.normalize();
		Quaternion<> q2norm_exp{
			0.10846522, 0.433860915, 0.216930457, 0.8677218312
		};
		q2norm_exp.equals(q2) or TESTFAIL;
	}
	{
		// Operator tests.
		Quaternion<> id{};
		Quaternion<> bla0{13, 37, 42, 235};
		bla0.equals(id * bla0) or TESTFAIL;

		Quaternion<> bla1{8, 16, 24, 32};
		Quaternion<> bla0_1_exp{21, 45, 50, 243};
		bla0_1_exp.equals(bla0 + bla1) or TESTFAIL;

		Quaternion<> bla1_2_exp{2, 8, 12, 16};
		bla1_2_exp.equals(bla1 / 4) or TESTFAIL;

		Quaternion<> bla1_3_exp{16, 64, 96, 128};
		bla1_3_exp.equals(bla1 * 8) or TESTFAIL;

		Quaternion<> bla1_4_exp{21, 53, 66, 267};
		bla1_4_exp.equals(bla0 + bla1) or TESTFAIL;

		Quaternion<> bla1_5_exp{5, 21, 18, 203};
		bla1_5_exp.equals(bla0 - bla1) or TESTFAIL;

		Quaternion<> bla2_exp{
			13 * 21 - 37 * 45 - 42 * 50 - 235 * 243,
			13 * 45 + 37 * 21 + 42 * 243 - 235 * 50,
			13 * 50 - 37 * 243 + 42 * 21 + 235 * 45,
			13 * 243 + 37 * 50 - 42 * 45 + 235 * 21
		};
		bla2_exp.equals(bla0 * bla0_1_exp) or TESTFAIL;

		(bla2_exp == bla2_exp) or TESTFAIL;
		(bla2_exp != bla0) or TESTFAIL;

		Quaternion<> bla0_neg_exp{-13, -37, -42, -235};
		bla0_neg_exp.equals(-bla0) or TESTFAIL;
	}
	{
		enum class axis {
			x, y, z
		};

		// Rotation tests
		auto rot_mat = [&](axis a, float am, bool deg=true) -> Matrix3 {
			if (deg) {
				am = (am * math::PI) / 180.0;
			}

			switch (a) {

			case axis::x:
				return {
					1, 0, 0,
					0, std::cos(am), -std::sin(am),
					0, std::sin(am), std::cos(am)
				};

			case axis::y:
				return {
					std::cos(am), 0, std::sin(am),
					0, 1, 0,
					-std::sin(am), 0, std::cos(am)
				};

			case axis::z:
				return {
					std::cos(am), -std::sin(am), 0,
					std::sin(am), std::cos(am), 0,
					0, 0, 1,
				};
			default:
				return {
					1, 0, 0,
					0, 1, 0,
					0, 0, 1,
				};
			};
		};

		// zero rotation = identity.
		Matrix3 rot = rot_mat(axis::x, 0, false);
		rot.equals(Matrix3::identity()) or TESTFAIL;
		Quaternion<> q_rot{rot};
		Quaternion<> q_rot_deg = Quaternion<>::from_rad(0, {1, 0, 0});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// 10 rad rotation
		rot = rot_mat(axis::x, 10, false);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_rad(10, {1, 0, 0});
		q_rot.equals_rad(q_rot_deg) or TESTFAIL;

		// wrong 90-rad-rotation:
		q_rot_deg = Quaternion<>::from_rad(90, {1, 0, 0});
		q_rot.equals_rad(q_rot_deg) and TESTFAIL;

		// 10 deg rotation
		rot = rot_mat(axis::x, 10);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(10, {1, 0, 0});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// 90 deg rotation
		rot = rot_mat(axis::x, 90);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(90, {1, 0, 0});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		rot = rot_mat(axis::y, 90);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(90, {0, 1, 0});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		rot = rot_mat(axis::z, 90);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(90, {0, 0, 1});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// -90 deg rotation
		rot = rot_mat(axis::y, -90);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(-90, {0, 1, 0});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// 45 deg rotation
		rot = rot_mat(axis::z, 45);
		q_rot = Quaternion<>{rot};
		q_rot_deg = Quaternion<>::from_deg(45, {0, 0, 1});
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// rotation combination
		rot = rot_mat(axis::z, 45) * rot_mat(axis::y, 60);
		q_rot = Quaternion<>{rot};
		q_rot_deg = (Quaternion<>::from_deg(45, {0, 0, 1}) *
		             Quaternion<>::from_deg(60, {0, 1, 0}));
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		rot = rot_mat(axis::z, 45) *
		      rot_mat(axis::y, 60) *
		      rot_mat(axis::x, -200);
		q_rot = Quaternion<>{rot};
		q_rot_deg = (Quaternion<>::from_deg(45, {0, 0, 1}) *
		             Quaternion<>::from_deg(60, {0, 1, 0}) *
		             Quaternion<>::from_deg(-200, {1, 0, 0}));
		q_rot.equals_deg(q_rot_deg) or TESTFAIL;

		// to_matrix tests
		rot = rot_mat(axis::x, 235);
		q_rot = Quaternion<>::from_deg(235, {1, 0, 0});
		rot.equals(q_rot.to_matrix()) or TESTFAIL;

		rot = rot_mat(axis::y, -55);
		q_rot = Quaternion<>::from_deg(-55, {0, 1, 0});
		rot.equals(q_rot.to_matrix()) or TESTFAIL;

		rot = rot_mat(axis::z, 64);
		q_rot = Quaternion<>::from_deg(64, {0, 0, 1});
		rot.equals(q_rot.to_matrix()) or TESTFAIL;
	}
	{
		Vector3 vec{5, 0, 0};
		Vector3 turned = Quaternion<>::from_deg(180, {0, 0, 1}) * vec;
		Vector3 turned_exp{-5, 0, 0};
		turned.equals(turned_exp) or TESTFAIL;

		// intentional fail:
		turned_exp = Vector3{-42, -42, -42};
		turned.equals(turned_exp) and TESTFAIL;

		// another turn
		vec = Vector3{1337, 42, 235};
		turned = Quaternion<>::from_deg(90, {1, 0, 0}) * vec;
		turned_exp = Vector3{1337, -235, 42};
		turned.equals(turned_exp) or TESTFAIL;
	}
}

}}} // openage::util::tests
