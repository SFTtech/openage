// Copyright 2015-2016 the openage authors. See copying.md for legal info.

import QtQuick 2.4

Item {
	id: root

	/*
	 * A paper texture.
	 */
	property url paperTextureSource: "image://by-filename/converted/interface/scr5b.slp.png.0"

	/*
	 * This texture is tiled along the item edges, bottom faces the inside.
	 * So, alpha should be zero for the pixels where to keep the paper, solid color - where it's torn off.
	 */
	property url edgeHmapSource: "image://by-filename/textures/torn_paper_edge.png.0"

	/*
	 * Which edges are torn off.
	 */
	property bool tornTop: true
	property bool tornBottom: true
	property bool tornLeft: true
	property bool tornRight: true

	/*
	 * Changing metrics.unit (currently bound to the font) scales all textures.
	 * So the details on the textures stay exactly the same relatively to metricsUnit.
	 */
	property real scaling: 8. / metricsUnit

	transform: Scale { xScale: 1. / root.scaling; yScale: 1. / root.scaling}

	Item {
		anchors.left: parent.left
		anchors.top: parent.top

		/*
		 * Do +.5 to avoid being less than parent due to truncation.
		 */
		width: (parent.width + .5) * root.scaling
		height: (parent.height + .5) * root.scaling

		Item {
			anchors.fill: parent

			/*
			 * Hack, so the child has 'visible' and 'layer.enabled'.
			 */
			clip: true

			Item {
				id: texturedPaper

				/*
				 * Hack, so that this item has 'visible' and 'layer.enabled'.
				 */
				width: parent.width
				height: parent.height
				x: parent.width

				layer.enabled: true

				Image {
					anchors.fill: parent
					source: root.paperTextureSource
					fillMode: Image.Tile

					/*
					 * Use an ellipse to darken parts that are far from the center.
					 */
					layer.enabled: true
					layer.wrapMode: ShaderEffectSource.Repeat
					layer.samplerName: "paperSource"
					layer.effect: ShaderEffect {
						fragmentShader: "
							uniform lowp sampler2D paperSource;
							varying highp vec2 qt_TexCoord0;

							const float exposure = .5;
							const float times = 3.;

							vec3 burntMidtones(vec3 src, float exposure, float times) {
								float factor = 1. + exposure * (.33);
								return pow(src.rgb, vec3(factor * times));
							}

							void main() {
								vec4 bkg = texture2D(paperSource, qt_TexCoord0);
								vec3 burnt = burntMidtones(bkg.rgb, exposure, times);

								float burnGradient = pow(pow((qt_TexCoord0.x - .5), 2.) + pow((qt_TexCoord0.y - 1.), 2.), 1.) * .9;

								gl_FragColor = vec4(mix(bkg.rgb, burnt, vec3(burnGradient)), 1.);
							}
						"
					}
				}
			}
		}

		/*
		 * Inverse QtGraphicalEffects.OpacityMask.
		 */
		Item {
			anchors.fill: parent

			layer.enabled: true

			layer.effect: ShaderEffect {
				property variant source: texturedPaper
				property variant mask: rectWithTornOffEdges

				fragmentShader: "
					uniform sampler2D source;
					uniform sampler2D mask;
					varying highp vec2 qt_TexCoord0;
					void main() {
						gl_FragColor = texture2D(source, qt_TexCoord0) * (1. - texture2D(mask, qt_TexCoord0).a);
					}
				"
			}
		}

		/*
		 * Alpha == 1.0 where the edges are torn off.
		 */
		Item {
			id: rectWithTornOffEdges

			anchors.fill: parent
			layer.enabled: true

			Image {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top

				source: root.edgeHmapSource
				fillMode: Image.Tile

				visible: root.tornTop
			}

			Image {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.x: rectWithTornOffEdges.width / 2
					origin.y: rectWithTornOffEdges.height / 2
					angle: 180
				}

				visible: root.tornBottom
			}

			Image {
				anchors.left: parent.left
				anchors.leftMargin: parent.height
				anchors.top: parent.top

				width: parent.height

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.y: rectWithTornOffEdges.height
					angle: -90
				}

				visible: root.tornLeft
			}

			Image {
				anchors.right: parent.right
				anchors.rightMargin: parent.height
				anchors.top: parent.top

				width: parent.height

				source: root.edgeHmapSource
				fillMode: Image.Tile

				transform: Rotation {
					origin.y: rectWithTornOffEdges.height
					origin.x: rectWithTornOffEdges.height
					angle: 90
				}

				visible: root.tornRight
			}

			visible: false
		}
	}

	/*
	 * Average character width that the paper will adapt to (by scaling its texture).
	 */
	/*
	 * Metric propagation.
	 */
	FontMetrics {
		id: fontMetrics
	}

	property int metricsUnit: metrics ? metrics.unit : fontMetrics.averageCharacterWidth
}
