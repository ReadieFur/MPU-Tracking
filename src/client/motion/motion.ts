import { Main } from "../assets/js/main.js";
import { Client } from "../assets/js/client.js";

class Motion
{
    client: Client;
    timeSinceLastUpdateElement: HTMLLabelElement;
    lastUpdateTimes:
    {
        ui: number;
        ax: number,
        ay: number,
        az: number,
        gx: number,
        gy: number,
        gz: number
    }
    domElements:
    {
        // accelerometer:
        // {
        //     x: HTMLLabelElement;
        //     y: HTMLLabelElement;
        //     z: HTMLLabelElement;
        // },
        gyroscope:
        {
            x: HTMLLabelElement;
            y: HTMLLabelElement;
            z: HTMLLabelElement;
        },
        calibrate: HTMLButtonElement;
    };
    calibrationData:
    {
        isCalibrating: boolean;
        gyroscope:
        {
            x: number;
            y: number;
            z: number;
        },
        accelerometer:
        {
            x: number;
            y: number;
            z: number;
        }
    };
    threeVariables:
    {
        canvasContainer: HTMLDivElement;
        scene: THREE.Scene;
        camera: THREE.Camera;
        renderer: THREE.WebGLRenderer;
        cube: THREE.Mesh;
    };

    constructor()
    {
        new Main();

        //#region Define variables
        this.client = new Client(Main.urlParams.get("ip"));

        this.timeSinceLastUpdateElement = Main.ThrowIfNullOrUndefined(document.querySelector("#timeSinceLastUpdate"));
        this.lastUpdateTimes =
        {
            ui: 0,
            ax: 0,
            ay: 0,
            az: 0,
            gx: 0,
            gy: 0,
            gz: 0
        };
        
        this.domElements =
        {
            // accelerometer:
            // {
            //     x: Main.ThrowIfNullOrUndefined(document.querySelector("#px")),
            //     y: Main.ThrowIfNullOrUndefined(document.querySelector("#py")),
            //     z: Main.ThrowIfNullOrUndefined(document.querySelector("#pz"))
            // },
            gyroscope:
            {
                x: Main.ThrowIfNullOrUndefined(document.querySelector("#rx")),
                y: Main.ThrowIfNullOrUndefined(document.querySelector("#ry")),
                z: Main.ThrowIfNullOrUndefined(document.querySelector("#rz"))
            },
            calibrate: Main.ThrowIfNullOrUndefined(document.querySelector("#calibrate"))
        };

        this.calibrationData =
        {
            isCalibrating: false,
            gyroscope:
            {
                x: 0,
                y: 0,
                z: 0
            },
            accelerometer:
            {
                x: 0,
                y: 0,
                z: 0
            }
        }

        this.threeVariables =
        {
            canvasContainer: Main.ThrowIfNullOrUndefined(document.querySelector("#canvasContainer")),
            scene: new THREE.Scene(),
            camera: new THREE.PerspectiveCamera(75, 16 / 9, 0.1, 1000),
            renderer: new THREE.WebGLRenderer({antialias: true}),
            cube: new THREE.Mesh(
                new THREE.BoxGeometry(5, 1, 4),
                new THREE.MultiMaterial([
                    // new THREE.MeshBasicMaterial({color:0xffffff}),
                    // new THREE.MeshBasicMaterial({color:0xcccccc}),
                    // new THREE.MeshBasicMaterial({color:0x999999}),
                    // new THREE.MeshBasicMaterial({color:0x656565}),
                    // new THREE.MeshBasicMaterial({color:0x313131}),
                    // new THREE.MeshBasicMaterial({color:0x262626})
                    new THREE.MeshBasicMaterial({color:0x00FF00}), //Y (Right face)
                    new THREE.MeshBasicMaterial({color:0xcccccc}), //Left face
                    new THREE.MeshBasicMaterial({color:0x0000FF}), //Z (Top face)
                    new THREE.MeshBasicMaterial({color:0x656565}), //Bottom face
                    new THREE.MeshBasicMaterial({color:0xFF0000}), //X (Front face)
                    new THREE.MeshBasicMaterial({color:0x262626}) //Back face
                ])
            )
        };
        //#endregion

        this.threeVariables.scene.background = new THREE.Color(0x000000);
        this.threeVariables.renderer.setSize(480, 320);
        this.threeVariables.canvasContainer.appendChild(this.threeVariables.renderer.domElement);
        this.threeVariables.scene.add(this.threeVariables.cube);
        this.threeVariables.camera.position.x = 4;
        this.threeVariables.camera.position.y = 2;
        this.threeVariables.camera.position.z = 4;
        this.threeVariables.camera.lookAt(this.threeVariables.scene.position);
        this.threeVariables.renderer.render(this.threeVariables.scene, this.threeVariables.camera);

        this.client.AddEndpoint("websocket");
        this.client.connections["websocket"].Connect();

        this.Begin();

        this.domElements.calibrate.addEventListener("click", () =>
        {
            this.domElements.calibrate.disabled = true;
            this.domElements.calibrate.innerHTML = "Calibrating...";
            this.Calibrate(15 * 2, "largest").then(() =>
            {
                console.log(this.calibrationData);
                this.domElements.calibrate.disabled = false;
                this.domElements.calibrate.innerHTML = "Re-Calibrate";
            });
        });
    }

    private Begin()
    {
        this.domElements.calibrate.disabled = true;
        this.domElements.calibrate.innerHTML = "Calibrating...";
        //15 samples a seconds for 10 seconds.
        this.Calibrate(15 * 2, "largest").then(() =>
        {
            console.log(this.calibrationData);
            this.client.connections["websocket"].AddEventListener("message", (e: IMotionData) => { this.OnMessage(e); });
            this.domElements.calibrate.disabled = false;
            this.domElements.calibrate.innerHTML = "Re-Calibrate";
        })
        .catch(() =>
        {
            this.Begin();
        });
    }

    private async Calibrate(samples: number, method: "average" | "largest"): Promise<boolean>
    {
        if (this.calibrationData.isCalibrating) { return new Promise<boolean>(() => { return false; }); }
        console.log(`Calibrating using ${samples} samples...`);
        return new Promise((resolve, reject) =>
        {
            this.calibrationData.isCalibrating = true;
            var samplesRecieved = 0;
            var samplesStore = [0, 0, 0, 0, 0, 0];
            var context: Motion = this;

            function UpdateSamples(e: IMotionData)
            {
                //Maybe instead of taking the average, check for the mean and make sure its above the half way mark. (I have the idea in my head but can't quite explain it properly).

                switch (method)
                {
                    case "average":
                        samplesStore[0] += e.gyroscope[0];
                        samplesStore[1] += e.gyroscope[1];
                        samplesStore[2] += e.gyroscope[2];
                        samplesStore[3] += e.accelerometer[0];
                        samplesStore[4] += e.accelerometer[1];
                        samplesStore[5] += e.accelerometer[2];
        
                        samplesRecieved++;
                        if (samplesRecieved >= samples)
                        {
                            context.client.connections["websocket"].RemoveEventListener("message", UpdateSamples);
        
                            context.calibrationData.gyroscope.x = samplesStore[0] / samples;
                            context.calibrationData.gyroscope.y = samplesStore[1] / samples;
                            context.calibrationData.gyroscope.z = samplesStore[2] / samples;
                            context.calibrationData.accelerometer.x = samplesStore[3] / samples;
                            context.calibrationData.accelerometer.y = samplesStore[4] / samples;
                            context.calibrationData.accelerometer.z = samplesStore[5] / samples;
        
                            context.calibrationData.isCalibrating = false;
        
                            resolve(true);
                        }
                        break;
                    case "largest":
                        //I don't know if this ABS works correctly for this, I am using it for negative numbers.
                        if (Math.abs(e.gyroscope[0]) > Math.abs(samplesStore[0])) { samplesStore[0] = e.gyroscope[0]; }
                        if (Math.abs(e.gyroscope[1]) > Math.abs(samplesStore[1])) { samplesStore[1] = e.gyroscope[1]; }
                        if (Math.abs(e.gyroscope[2]) > Math.abs(samplesStore[2])) { samplesStore[2] = e.gyroscope[2]; }
                        if (Math.abs(e.accelerometer[0]) > Math.abs(samplesStore[3])) { samplesStore[3] = e.accelerometer[0]; }
                        if (Math.abs(e.accelerometer[1]) > Math.abs(samplesStore[4])) { samplesStore[4] = e.accelerometer[1]; }
                        if (Math.abs(e.accelerometer[2]) > Math.abs(samplesStore[5])) { samplesStore[5] = e.accelerometer[2]; }

                        samplesRecieved++;
                        if (samplesRecieved >= samples)
                        {
                            context.client.connections["websocket"].RemoveEventListener("message", UpdateSamples);

                            context.calibrationData.gyroscope.x = samplesStore[0];
                            context.calibrationData.gyroscope.y = samplesStore[1];
                            context.calibrationData.gyroscope.z = samplesStore[2];
                            context.calibrationData.accelerometer.x = samplesStore[3];
                            context.calibrationData.accelerometer.y = samplesStore[4];
                            context.calibrationData.accelerometer.z = samplesStore[5];
            
                            context.calibrationData.isCalibrating = false;

                            resolve(true);
                        }
                        break;
                }
            }

            function WSError(e: Event)
            {
                context.client.connections["websocket"].RemoveEventListener("message", UpdateSamples);
                context.client.connections["websocket"].RemoveEventListener("error", WSError);
                context.client.connections["websocket"].RemoveEventListener("close", WSError);
                context.calibrationData.isCalibrating = false;
                reject(false);
            }

            this.client.connections["websocket"].AddEventListener("message", UpdateSamples);
            this.client.connections["websocket"].AddEventListener("error", WSError);
            this.client.connections["websocket"].AddEventListener("close", WSError);
        });
    }

    //I am reading the time in MS as close to the updates as I can to make the data as accurate as possible.
    private OnMessage(e: IMotionData)
    {
        if (this.calibrationData.isCalibrating)
        {
            const now = new Date().getTime();
            this.threeVariables.cube.rotation.x = 0;
            this.threeVariables.cube.rotation.y = 0;
            this.threeVariables.cube.rotation.z = 0;
            this.domElements.gyroscope.x.innerText = "0rad/s";
            this.domElements.gyroscope.y.innerText = "0rad/s";
            this.domElements.gyroscope.z.innerText = "0rad/s";
            this.lastUpdateTimes =
            {
                ui: now,
                ax: now,
                ay: now,
                az: now,
                gx: now,
                gy: now,
                gz: now
            }
            this.threeVariables.renderer.render(this.threeVariables.scene, this.threeVariables.camera);
            return;
        }

        //I have had to flip some of these readings around so that they correspond to the same axis as the gyroscope.
        //Of course this also depends on how the sensor is mounted.

        const gxNow = new Date().getTime();
        if (Math.abs(e.gyroscope[0]) > Math.abs(this.calibrationData.gyroscope.x))
        {
            this.threeVariables.cube.rotation.z += -(e.gyroscope[0] * ((gxNow - this.lastUpdateTimes.gx) * 0.001));
            this.domElements.gyroscope.x.innerText = `${e.gyroscope[0]}rad/s`;
        }
        else
        {
            this.domElements.gyroscope.x.innerText = "0rad/s";
        }
        this.lastUpdateTimes.gx = gxNow;

        const gyNow = new Date().getTime();
        if (Math.abs(e.gyroscope[1]) > Math.abs(this.calibrationData.gyroscope.y))
        {
            this.threeVariables.cube.rotation.x += -(e.gyroscope[1] * ((gyNow - this.lastUpdateTimes.gy) * 0.001));
            this.domElements.gyroscope.y.innerText = `${e.gyroscope[1]}rad/s`;
        }
        else
        {
            this.domElements.gyroscope.y.innerText = "0rad/s";
        }
        this.lastUpdateTimes.gy = gyNow;

        const gzNow = new Date().getTime();
        if (Math.abs(e.gyroscope[2]) > Math.abs(this.calibrationData.gyroscope.z))
        {
            this.threeVariables.cube.rotation.y += e.gyroscope[2] * ((gzNow - this.lastUpdateTimes.gz) * 0.001);
            this.domElements.gyroscope.z.innerText = `${e.gyroscope[2]}rad/s`;
        }
        else
        {
            this.domElements.gyroscope.z.innerText = "0rad/s";
        }
        this.lastUpdateTimes.gz = gzNow;

        //Instead of updating the frames from here, set a frame rate and then smooth between the inputs.
        //Though this would require predicting where the next position would be.
        //So I could wait a frame behind but that would cause a lot of lag.
        this.threeVariables.renderer.render(this.threeVariables.scene, this.threeVariables.camera);

        const uiUpdateTime = new Date().getTime();
        this.timeSinceLastUpdateElement.innerText = `${uiUpdateTime - this.lastUpdateTimes.ui}ms`;
        this.lastUpdateTimes.ui = uiUpdateTime;
    }
}
new Motion();

interface IMotionData
{
    gyroscope: [0, 0, 0];
    accelerometer: [0, 0, 0];
}